#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>

void kheap_init(void);
void* kmalloc(size_t n_bytes);
void* krealloc(void* old, size_t n_bytes);
void* kmalloc_user(size_t n_bytes);
void kfree(void* block);

#endif
