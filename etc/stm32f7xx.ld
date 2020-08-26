ENTRY(_start);

__stack_top = 0x2007D000;

MEMORY {
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 2M
    SRAM1(rwx)      : ORIGIN = 0x20020000, LENGTH = 368K
    SRAM2(rwx)      : ORIGIN = 0x2007C000, LENGTH = 16K
    DTCM(rwx)       : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS {
    .vectors : ALIGN(4) {
        KEEP(*(.vectors))
    } >FLASH

    .text : {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    __data_start_src = .;

    .data : AT(__data_start_src) {
        . = ALIGN(4);
        __data_start = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        __data_end = .;
    } >SRAM1

    .bss : {
        . = ALIGN(4);
        __bss_start = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        __bss_end = .;
    } >SRAM1
}
