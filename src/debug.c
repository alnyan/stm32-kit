#include <stddef.h>
#include <stdint.h>
typedef int32_t ssize_t;
#include <stdarg.h>

#include "stm32f7/usart.h"
#include "stm32f7/gpio.h"
#include "stm32f7/rcc.h"

#include "libc/string.h"
#include "util.h"

static const char *printf_hex_low  = "0123456789abcdef";
static const char *printf_hex_high = "0123456789ABCDEF";

static int isdigit(int ch) {
    return ch >= '0' && ch <= '9';
}
static int isprint(int ch) {
    return ch >= ' ' && ch < 127;
}

static inline size_t num2ascii(char *dst, uint32_t num, int base, const char *charset) {
    if (!num) {
        dst[0] = '0';
        dst[1] = 0;
        return 1;
    }
    size_t c = 0, i, j;

    while (num) {
        dst[c++] = charset[num % base];
        num /= base;
    }

    i = 0;
    j = c - 1;
    while (i < j) {
        dst[i] ^= dst[j];
        dst[j] ^= dst[i];
        dst[i++] ^= dst[j--];
    }

    dst[c] = 0;

    return c;
}

enum printf_len {
    PL_0,
    PL_hh,
    PL_h,
    PL_l,
    PL_ll,
    PL_j,
    PL_z,
    PL_t,
    PL_L
};

int __libc_vprintf(const char *format,
                   void *ctx,
                   int (*out)(void *ctx, const char *text, size_t len),
                   va_list ap) {
    (void) ap;
    char tmp[32];
    union {
        uint32_t value_unsigned;
        int32_t value_signed;
        char value_char;
        const char *value_str;
    } pv;
    size_t l;
    int count = 0;
    char ch;
    enum printf_len lenmod;
    int field_width;
    int neg_field_width;
    char prefix;
    char pad_char;

    while ((ch = *format++)) {
        if (ch == '%') {
            field_width = 0;
            neg_field_width = 0;
            pad_char = ' ';
            lenmod = PL_0;
            prefix = 0;

            while ((ch = *format)) {
                switch (ch) {
                case '\'':
                case '#':
                    // XXX: Non-POSIX: I don't need that lol
                    ++format;
                    continue;
                case ' ':
                    if (prefix != '+') {
                        prefix = ' ';
                    }
                    ++format;
                    continue;
                case '0':
                    pad_char = '0';
                    ++format;
                    continue;
                case '-':
                    neg_field_width = 1;
                    ++format;
                    continue;
                case '+':
                    prefix = '+';
                    ++format;
                    continue;
                default:
                    break;
                }
                break;
            }

            if (*format == '*') {
                field_width = va_arg(ap, int);
                ++format;
            } else {
                while ((ch = *format) && isdigit(ch)) {
                    field_width *= 10;
                    field_width += ch - '0';
                    ++format;
                }
            }
            if (neg_field_width) {
                field_width = -field_width;
            }

            // TODO: precision
            if (*format == '.') {
                ++format;
                if (*format == '*') {
                    (void) va_arg(ap, int);
                    ++format;
                } else {
                    while ((ch = *format) && isdigit(ch)) {
                        ++format;
                    }
                }
            }

            // Collect length modifier
            while ((ch = *format)) {
                if (ch == 'h') {
                    if (lenmod == PL_0) {
                        lenmod = PL_h;
                        ++format;
                        continue;
                    } else if (lenmod == PL_h) {
                        lenmod = PL_hh;
                        ++format;
                        continue;
                    }
                }
                if (ch == 'l') {
                    if (lenmod == PL_0) {
                        lenmod = PL_l;
                        ++format;
                        continue;
                    } else if (lenmod == PL_l) {
                        lenmod = PL_ll;
                        ++format;
                        continue;
                    }
                }
                switch (ch) {
                case 'z':
                    lenmod = PL_z;
                    ++format;
                    break;
                case 'j':
                    lenmod = PL_j;
                    ++format;
                    break;
                case 't':
                    lenmod = PL_t;
                    ++format;
                    break;
                case 'L':
                    lenmod = PL_L;
                    ++format;
                    break;
                default:
                    break;
                }
                break;
            }

            ch = *format++;
            if (ch == 'd' ||
                ch == 'u' ||
                ch == 'x' ||
                ch == 'X' ||
                ch == 'o' ||
                ch == 'i' ||
                ch == 'p') {
                int is_signed = (ch == 'd' || ch == 'i');
                if (ch == 'p') {
                    pv.value_unsigned = (uintptr_t) va_arg(ap, void *);
                } else {
                    if (is_signed) {
                        switch (lenmod) {
                        case PL_l:
                            pv.value_signed = va_arg(ap, long);
                            break;
                        case PL_ll:
                            pv.value_signed = va_arg(ap, long long);
                            break;
                        case PL_h:
                            pv.value_signed = va_arg(ap, int);
                            break;
                        case PL_hh:
                            pv.value_signed = va_arg(ap, int);
                            break;
                        case PL_z:
                            pv.value_signed = va_arg(ap, ssize_t);
                            break;
                        case PL_j:
                            pv.value_unsigned = va_arg(ap, intmax_t);
                            break;
                        case PL_t:
                            pv.value_unsigned = va_arg(ap, ptrdiff_t);
                            break;
                        default:
                            pv.value_signed = va_arg(ap, int);
                            break;
                        }
                    } else {
                        switch (lenmod) {
                        case PL_l:
                            pv.value_unsigned = va_arg(ap, unsigned long);
                            break;
                        case PL_ll:
                            pv.value_unsigned = va_arg(ap, unsigned long long);
                            break;
                        case PL_h:
                            pv.value_unsigned = va_arg(ap, unsigned int);
                            break;
                        case PL_hh:
                            pv.value_unsigned = va_arg(ap, unsigned int);
                            break;
                        case PL_z:
                            pv.value_unsigned = va_arg(ap, size_t);
                            break;
                        case PL_j:
                            pv.value_unsigned = va_arg(ap, uintmax_t);
                            break;
                        case PL_t:
                            pv.value_unsigned = va_arg(ap, uintptr_t);
                            break;
                        default:
                            pv.value_unsigned = va_arg(ap, unsigned int);
                            break;
                        }
                    }
                }

                if (is_signed && pv.value_signed < 0) {
                    pv.value_signed = -pv.value_signed;
                    prefix = '-';
                }
                int base;
                switch (ch) {
                case 'x':
                case 'X':
                case 'p':
                    base = 16;
                    break;
                case 'd':
                case 'i':
                case 'u':
                default:
                    base = 10;
                    break;
                }
                if (ch == 'p') {
                    l = 2;
                    tmp[0] = '0';
                    tmp[1] = 'x';
                } else {
                    l = !!prefix;
                    if (l) {
                        tmp[0] = prefix;
                    }
                }
                l += num2ascii(tmp + l,
                               pv.value_unsigned,
                               base,
                               ch == 'X' ? printf_hex_high : printf_hex_low);

                if (field_width > 0 && l < (size_t) field_width) {
                    for (size_t i = 0; i < field_width - l; ++i) {
                        if (out(ctx, &pad_char, 1) != 0) {
                            goto end;
                        }
                        ++count;
                    }
                }

                if (out(ctx, tmp, l) != 0) {
                    goto end;
                }
                count += l;
            } else if (ch == 'c') {
                // TODO: wchar_t
                pv.value_char = va_arg(ap, int);
                l = 1;

                if (out(ctx, &pv.value_char, 1) != 0) {
                    goto end;
                }
                ++count;
            } else if (ch == 's') {
                // TODO: wchar_t
                pv.value_str = va_arg(ap, const char *);
                if (!pv.value_str) {
                    // XXX: I'm not, but SUSv4 doesn't specify what to do
                    //      when %s argument is NULL
                    pv.value_str = "(nil)";
                }

                l = strlen(pv.value_str);
                if (field_width > 0 && l < (size_t) field_width) {
                    for (size_t i = 0; i < field_width - l; ++i) {
                        if (out(ctx, &pad_char, 1) != 0) {
                            goto end;
                        }
                        ++count;
                    }
                }
                if (out(ctx, pv.value_str, l) != 0) {
                    goto end;
                }
                count += l;
            } else {
                if (out(ctx, &ch, 1) != 0) {
                    goto end;
                }
                ++count;
                continue;
            }

            // Right padding, if any
            field_width = -field_width;
            if (field_width > 0 && l < (size_t) field_width) {
                for (size_t i = 0; i < field_width - l; ++i) {
                    if (out(ctx, &pad_char, 1) != 0) {
                        goto end;
                    }
                    ++count;
                }
            }
        } else {
            if (out(ctx, &ch, 1) != 0) {
                goto end;
            }
            ++count;
        }
    }

end:
    return count;
}

static void debug_puts(const char *text, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (text[i] == '\n') {
            usart_tx(usart2, '\r');
        }
        usart_tx(usart2, text[i]);
    }
}

static int debug_printf_out(void *ctx, const char *text, size_t count) {
    (void) ctx;
    debug_puts(text, count);
    return 0;
}

void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __libc_vprintf(fmt, 0, debug_printf_out, args);
    va_end(args);
}

void debug_dump(const void *block, size_t count) {
    size_t n_lines = (count + 15) / 16;
    const char *bytes = block;

    for (size_t l = 0; l < n_lines; ++l) {
        printf("%08x: ", l * 16);

        for (size_t i = 0; i < 8; ++i) {
            if (i * 2 + l * 16 < count) {
                uint16_t word = *(const uint16_t *) (bytes + i * 2 + l * 16);

                usart_tx(usart2, printf_hex_high[(word >> 4) & 0xF]);
                usart_tx(usart2, printf_hex_high[word & 0xF]);

                usart_tx(usart2, printf_hex_high[word >> 12]);
                usart_tx(usart2, printf_hex_high[(word >> 8) & 0xF]);
            } else {
                usart_tx(usart2, ' ');
                usart_tx(usart2, ' ');
                usart_tx(usart2, ' ');
                usart_tx(usart2, ' ');
            }
            usart_tx(usart2, ' ');
        }

        usart_tx(usart2, '|');
        usart_tx(usart2, ' ');
        for (size_t i = 0; i < 16; ++i) {
            if (i + l * 16 < count) {
                char c = bytes[i + l * 16];

                if (isprint(c)) {
                    usart_tx(usart2, c);
                } else if (c != 0) {
                    usart_tx(usart2, ' ');
                } else {
                    usart_tx(usart2, '.');
                }
            }
        }
        usart_tx(usart2, '\r');
        usart_tx(usart2, '\n');
    }
}
