#include <timer/pit.h>
#include <arch/io.h>


void pit_init(void) {
	const uint16_t DIVISOR = 1193180 / PIT_FREQUENCY_HZ;
	outportb(0x43, 0x36);					// Command byte.
	outportb(0x40, DIVISOR & 0xFF);
	outportb(0x40, DIVISOR >> 8);
}
