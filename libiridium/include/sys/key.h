#ifndef LIBIRIDIUM__SYS__KEY_H
#define LIBIRIDIUM__SYS__KEY_H


#define KEYSTROKE_EXTRACT_SCANCODE(keystroke_payload) (keystroke_payload & 0xFFFF)
#define KEYSTROKE_EXTRACT_CHARACTER(keystroke_payload) ((keystroke_payload >> 16) & 0xFF)
#define KEYSTROKE_EXTRACT_PRESSED(keystroke_payload) (keystroke_payload & (1 << 24) >> 24)


#endif
