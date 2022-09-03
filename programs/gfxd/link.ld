ENTRY(main)

SECTIONS {
    . = 0x9000;
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
