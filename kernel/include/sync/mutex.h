#ifndef SYNC_MUTEX_H
#define SYNC_MUTEX_H

#include <stdint.h>

#define MUTEX_UNLOCKED 0


typedef uint8_t MUTEX_T;


void mutex_acquire(MUTEX_T* lock);
void mutex_release(MUTEX_T* lock);


#endif
