ENTRY(main)

SECTIONS {
    . = 0x2000;
    .text 0x2000 : {
        *(.text)
    }
}
