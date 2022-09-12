#ifndef MM_PMM_H
#define MM_PMM_H

#include <stdint.h>

void pmm_init(void);


/*
 *  Allocates a physical frame.
 *  Returns 0 if out of frames.
 *
 */

uint64_t pmm_alloc(void);

/*
 * Frees a physical frame.
 *
 */

void pmm_free(uint64_t base);

#endif
