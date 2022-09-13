#ifndef LIBIRIDIUM__STRING_H
#define LIBIRIDIUM__STRING_H

#include <stddef.h>
#include <stdint.h>


size_t strlen(const char* str);
uint8_t bufcmp(const char* buf, const char* cmpstr, size_t cmp_len);

#endif
