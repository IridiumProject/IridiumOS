#include <arch/cpu/reset.h>
#include <arch/io.h>

void cpu_reset(void) {
	outportb(0x64, 0xFE);
}
