#ifndef DEBUG_H
#define DEBUG_H

#include <common/panic.h>
#include <common/log.h>
#include <common/asm.h>

// Switch to 1 for debug messages.
#define SHOW_DEBUG_MESSAGES 0

#if SHOW_DEBUG_MESSAGES
#define DEBUG
#endif


#define ASSERT(condition) \
  if (!(condition)) { kprintf(KERN_PANIC "![BUG]! ASSERTION %s FAILED! (%s:%d)\nREPORT THIS BUG AND INCLUDE A SCREENSHOT.\n", #condition, __FILE__, __LINE__); CLI; HLT; }


#endif
