#ifndef LIBIRIDIUM__UNISTD__H
#define LIBIRIDIUM__UNISTD__H

#include <stdint.h>


// Locks.

#define MUTEX_UNLOCKED 0


typedef uint8_t MUTEX_T;

void mutex_acquire(MUTEX_T* lock);
void mutex_release(MUTEX_T* lock);


#endif
