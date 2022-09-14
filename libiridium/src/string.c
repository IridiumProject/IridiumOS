#include <string.h>


size_t strlen(const char* str) {
    size_t i = 0;
    while (str[i++]);
    return i-1;
}


uint8_t bufcmp(const char* buf, const char* cmpstr, size_t cmp_len) {
    if (strlen(cmpstr) != cmp_len) {
        return 1;
    }

    for (size_t i = 0; i < cmp_len; ++i) {
        if (*buf++ != *cmpstr++) {
            return 1;
        }
    }

    return 0;
}
