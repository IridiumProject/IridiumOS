#ifndef PROC_H
#define PROC_H

#include <stdint.h>

#define PCONTEXT_SIZE 8


typedef uint16_t PID_T;

typedef enum {
    PSTATE_BLOCKED,
    PSTATE_RUNNING,
    PSTATE_READY
} PSTATE_T;


typedef enum {
    PCTX_RBP = 0,
    PCTX_RSP = 1,
    PCTX_RBX = 2,
    PCTX_RDX = 3,
    PCTX_RCX = 4,
    PCTX_RAX = 5,
    PCTX_RIP = 6,
    PCTX_CR3 = 7,
} PCONTEXT_INDEX;

struct Process {
    PID_T pid;                          // Process identifier.
    PSTATE_T state;                     // Process state.
    uint64_t context[PCONTEXT_SIZE];    // Context (registers and address space).
    struct Process* prev;               // Pointer to process on the prev.
    struct Process* next;               // Pointer to process on the next.
};

__attribute__((naked)) void proc_init(void);

// Assembly helpers for proc.asm
uint64_t* proc_get_context(struct Process* root);
struct Process* proc_get_next(struct Process* root);

#endif
