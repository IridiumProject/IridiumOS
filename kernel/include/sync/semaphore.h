#ifndef SYNC_SEMAPHORE_H
#define SYNC_SEMAPHORE_H

#include <sync/spinlock.h>
#include <stddef.h>


#define SEMAPHORE_INIT(max_n_cnt)    \
    {                                \
        .n = max_n_cnt,              \
        .max_n = max_n_cnt,          \
        .lock = 0                    \
    };


typedef struct {
    _Atomic size_t n;
    size_t max_n;
    SPINLOCK_T lock;
} SEMAPHORE_T;


void semaphore_up(SEMAPHORE_T* sem);
void semaphore_down(SEMAPHORE_T* sem);

#endif
