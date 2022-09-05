#include <string.h>


size_t strlen(const char* str) {
    size_t i;
    while (str[i++]);
    return i - 1;
}
