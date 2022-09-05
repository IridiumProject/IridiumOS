#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <proc/drvmaster.h>
#include <proc/perm.h>
#include <common/errno.h>

#define PCONTEXT_SIZE 8

// If changing any of these typedefs, update 
// libiridium/include/sys/process.h
typedef int16_t PID_T;
typedef uint32_t PPERM_T;

typedef enum {
    PSTATE_BLOCKED = 0,
    PSTATE_RUNNING = 1,
    PSTATE_READY = 2
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
    PID_T pid;                                                  // Process identifier.
    PSTATE_T state;                                             // Process state.
    PPERM_T perm_mask;                                          // Premission bitmask.
    uint64_t context[PCONTEXT_SIZE];                            // Context (registers and address space).
    uint16_t n_slave_driver_groups;                             // The amount of driver groups this process owns.
    DRIVER_TYPE_T slave_driver_groups[DRVMASTER_MAX_SLAVES];    // Driver group this process owns (used by daemons, if not used: DRIVERTYPE_NONE).
    struct Process* prev;                                       // Pointer to process on the prev.
    struct Process* next;                                       // Pointer to process on the next.
};

__attribute__((naked)) void proc_init(void);

// Assembly helpers for proc.asm
uint64_t* proc_get_context(struct Process* root);
struct Process* proc_get_next(struct Process* root);
void proc_set_state(struct Process* root, PSTATE_T state);

// Permission related stuff.
ERRNO_T perm_grant(PID_T pid, PPERM_T perms);
ERRNO_T perm_revoke(PID_T pid, PPERM_T perms);


// Process creation/deletion.

// Returns < 0 as error code if something goes wrong.
PID_T spawn(void* rip, PPERM_T permissions);


#endif
