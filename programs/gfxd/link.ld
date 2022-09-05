ENTRY(main)

SECTIONS {
    . = 0x2000;
    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}
