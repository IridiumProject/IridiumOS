ENTRY(main)

SECTIONS {
    . = 0xB000;
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
