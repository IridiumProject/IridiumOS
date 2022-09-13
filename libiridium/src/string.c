#include <string.h>


size_t strlen(const char* str) {
    size_t i;
    while (str[i++]);
    return i - 1;
}


uint8_t strncmp(const char* str1, const char* str2, size_t n) { 
    for (size_t i = 0; i < n; ++i) {
        if (str1[i] != str2[i]) {
            return 1;
        }
    }

    return 0;
}
