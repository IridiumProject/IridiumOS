#include <drivers/ps2/keyboard.h>
#include <arch/io.h>
#include <arch/x86/lapic.h>


/*
 *  Bits 15-0: Scancode
 *  Bits 23-16: Character
 *  Bit 24: Pressed (1: true, 0: false).
 *
 *  TODO: Buffer keystrokes later.
 */

static uint32_t last_keystroke = 0;

void __attribute__((interrupt)) irq1_isr(void* stack_frame) {
    const char* const SC_ASCII = "\x00\x1B" "1234567890-=" "\x08"
        "\x00" "qwertyuiop[]" "\x0D\x1D" "asdfghjkl;'`" "\x00" "\\"
        "zxcvbnm,./" "\x00\x00\x00" " ";

    // Read in the scancode.
    uint16_t scancode = inportb(0x60);
    char character = SC_ASCII[scancode];
    uint8_t pressed = 0;

    if (!(scancode & 0x80)) {
        if ((SC_ASCII[scancode] >= 'a' && SC_ASCII[scancode] <= 'z') || 
                (SC_ASCII[scancode] >= 'A' && SC_ASCII[scancode] <= 'Z') || 
                (SC_ASCII[scancode] >= '0' && SC_ASCII[scancode]) <= '9' || 
                (SC_ASCII[scancode] >= '!' && SC_ASCII[scancode] <= '.') || 
                (SC_ASCII[scancode] >= '[' && SC_ASCII[scancode] <= '`')) {
            pressed = 1;
        }
    }

    last_keystroke = (pressed << 24 | character << 16 | scancode);
    lapic_send_eoi();
}


uint32_t ps2_fetch_keystroke(void) {
    uint32_t tmp = last_keystroke;
    last_keystroke = 0;
    return tmp;
}
