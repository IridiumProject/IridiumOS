#include <sync/semaphore.h>

void semaphore_up(SEMAPHORE_T* sem) {
    if (sem->n < sem->max_n) {
        ++sem->n;
    }

    if (sem->n == 1 && sem->type == SEMAPHORE_LOCK) {
        spinlock_release(&sem->lock);
    }
}

void semaphore_down(SEMAPHORE_T* sem) {
    ASSERT(sem->type == SEMAPHORE_LOCK);    // Must be a lock-type semaphore.
    if (sem->n > 0) {
        --sem->n;
    } else {
        spinlock_acquire(&sem->lock);
    }
}
