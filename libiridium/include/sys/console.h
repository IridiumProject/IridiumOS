#ifndef LIBIRIDIUM__SYS__CONSOLE_H
#define LIBIRIDIUM__SYS__CONSOLE_H

#include <sys/_syscall.h>
#include <errno.h>
#include <stdint.h>


#define RGB2HEX(r, g, b) (r << 16 | g << 8 | b)
#define DEFAULT_COLOR RGB2HEX(255, 255, 0)


_syscallab ERRNO_T con_out_u32(const char* str, uint32_t color);


#endif
