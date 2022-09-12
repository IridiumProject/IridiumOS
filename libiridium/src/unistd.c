#include <unistd.h>

// Locks.

void mutex_acquire(MUTEX_T* lock) {
    while (__atomic_test_and_set(lock, __ATOMIC_ACQUIRE));
}

void mutex_release(MUTEX_T* lock) {
    __atomic_clear(lock, __ATOMIC_RELEASE);
}
