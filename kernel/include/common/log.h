#ifndef LOG_H
#define LOG_H
#include <stdarg.h>

#define KINFO "\e[0;32m"
#define BLUE "\e[0;34m"
#define YELLOW "\e[0;33m"
#define PURPLE "\e[0;35m"
#define CYAN "\e[0;36m"
#define WHITE "\e[0;37m"
#define BOLD_YELLOW "\e[1;33m"

#define STATUS_MSG_COLOR CYAN
#define STATUS_MSG_OK_COLOR KINFO

void kprintf(char* fmt, ...);
void clear_term(void);

#endif
