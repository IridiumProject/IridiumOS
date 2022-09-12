#ifndef STRING_H
#define STRING_H


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

size_t strlen(const char* str);
bool memcmp(const char* str1, const char* str2, size_t n);
char* dec2str(int dec);
uint8_t* strncpy(uint8_t* dst, const uint8_t* src, const uint8_t len);
void memcpy(uint8_t* dst, const uint8_t* src, size_t len);
uint8_t* hex2str(uint32_t hex_num);
void memzero(void* ptr, size_t n);
void memset(void* ptr, uint64_t data, size_t n);
uint8_t strcmp(const char* str1, const char* str2);
uint8_t strncmp(const char* str1, const char* str2, size_t n);

#endif
