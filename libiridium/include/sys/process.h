#ifndef LIBIRIDIUM__SYS__PROCESS_H
#define LIBIRIDIUM__SYS__PROCESS_H

#include <stdint.h>


typedef int16_t PID_T;
typedef uint32_t PPERM_T;


/*
 *  Returns PID of most recently created
 *  process.
 */

PID_T get_top_pid(void);

#endif
