#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>


typedef size_t KHEAP_STATUS_T;


typedef enum {
  KHEAP_INVALID_FREE = (1 << 0),
  KHEAP_NO_MEMORY = (1 << 1)
} KHEAP_ERR;


void kheap_init(void);
void* kmalloc(size_t n_bytes);
void* krealloc(void* old, size_t n_bytes);
void* kmalloc_user(size_t n_bytes);
void kfree(void* block);


/*
 *  Mostly used for debugging,
 *  returns a status code on how
 *  a heap operation went.
 *
 */

KHEAP_STATUS_T kheap_status(void);


#endif
