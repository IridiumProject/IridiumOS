#ifndef SYNC_SEMAPHORE_H
#define SYNC_SEMAPHORE_H

#include <sync/spinlock.h>
#include <common/debug.h>
#include <stddef.h>


#define SEMAPHORE_INIT(sem_type, max_n_cnt)    \
    {                                          \
        .n = max_n_cnt,                        \
        .max_n = max_n_cnt,                    \
        .lock = 0,                             \
        .sem_type = type                       \
    };


#define SEMAPHORE_WRITE_INIT(sem_type, max_n_cnt, sem)    \
    sem.n = max_n_cnt;                                    \
    sem.max_n = max_n_cnt;                                \
    sem.lock = 0;                                         \
    sem.type = sem_type;


// !!!NOTE!!!: USE IF SEMAPHORE TYPE IS SEMAPHORE_REJECT.
#define REJ_SEMAPHORE_DOWN(semptr)                  \
    ASSERT((semptr)->type == SEMAPHORE_REJECT);     \
    if ((semptr)->n > 0) {                          \
        --(semptr)->n;                              \
    } else {                                        \
       return -EREJECTED;                           \
    }


typedef enum {
    SEMAPHORE_LOCK,    // Locks if n == 0.
    SEMAPHORE_REJECT,  // Function must have return type of ERRNO_T (returns error if n == 0).
} SEMAPHORE_TYPE_T;


typedef struct {
    _Atomic size_t n;
    size_t max_n;
    SPINLOCK_T lock;
    SEMAPHORE_TYPE_T type;
} SEMAPHORE_T;


void semaphore_up(SEMAPHORE_T* sem);
void semaphore_down(SEMAPHORE_T* sem);

#endif
