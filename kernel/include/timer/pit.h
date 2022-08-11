#ifndef PIT_H
#define PIT_H

#include <stdint.h>

#define PIT_FREQUENCY_HZ 100

// S prefix means safe.
#define S_PIT_SLEEP(n_ticks) __asm__ __volatile__("cli"); \
							 pit_sleep(n_ticks);          
							 // STI is called before the end of pit_sleep()

void pit_init(void);
void pit_sleep(uint16_t n_ticks);

#endif
