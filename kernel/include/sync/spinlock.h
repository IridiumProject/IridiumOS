#ifndef SYNC_SPINLOCK_H
#define SYNC_SPINLOCK_H

#include <stdint.h>


typedef uint8_t SPINLOCK_T;


void spinlock_acquire(SPINLOCK_T* lock);
void spinlock_release(SPINLOCK_T* lock);



#endif
