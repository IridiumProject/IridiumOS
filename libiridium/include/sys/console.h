#ifndef LIBIRIDIUM__SYS__CONSOLE_H
#define LIBIRIDIUM__SYS__CONSOLE_H

#include <sys/_syscall.h>
#include <errno.h>
#include <stdint.h>

#define CONSOLE_BLUE "\e[0;34m"
#define CONSOLE_YELLOW "\e[0;33m"
#define CONSOLE_PURPLE "\e[0;35m"
#define CONSOLE_CYAN "\e[0;36m"
#define CONSOLE_WHITE "\e[0;37m"
#define CONSOLE_BOLD_YELLOW "\e[1;33m"
#define CONSOLE_CLEAR "\033[H\033[2J\033[3J"


_syscallab ERRNO_T con_out_u32(const char* str, uint32_t arg);


#endif
