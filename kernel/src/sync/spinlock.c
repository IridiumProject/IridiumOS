#include <sync/spinlock.h>


#define SPINLOCK_ACQUIRED 1
#define SPINLOCK_RELEASED 0


void spinlock_acquire(SPINLOCK_T* lock) {
    __atomic_store_n(lock, SPINLOCK_ACQUIRED, __ATOMIC_SEQ_CST);
    while (*lock);
}


void spinlock_release(SPINLOCK_T* lock) {
    __atomic_store_n(lock, SPINLOCK_RELEASED, __ATOMIC_RELEASE);
}
