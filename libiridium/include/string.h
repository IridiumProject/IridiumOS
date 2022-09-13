#ifndef LIBIRIDIUM__STRING_H
#define LIBIRIDIUM__STRING_H

#include <stddef.h>
#include <stdint.h>


size_t strlen(const char* str);
uint8_t strncmp(const char* str1, const char* str2, size_t n);             // Returns 0 if equal otherwise 1.


#endif
