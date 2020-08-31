#include "libc/string.h"

size_t strlen(const char *str) {
    size_t len;
    for (len = 0; str[len]; ++len);
    return len;
}

int strcmp(const char *a, const char *b) {
    for (;; ++a, ++b) {
        int delta = *a - *b;
        if (delta) {
            return delta;
        }
        if (!*a) {       // Implies !*b
            break;
        }
    }
    return 0;
}

char *strchr(const char *str, int chr) {
    for (; *str; ++str) {
        if (*str == chr) {
            return (char *) str;
        }
    }
    return NULL;
}

