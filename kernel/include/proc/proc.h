#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <stddef.h>
#include <proc/drvmaster.h>
#include <proc/perm.h>
#include <proc/psignal.h>
#include <common/errno.h>
#include <sync/semaphore.h>
#include <sync/mutex.h>

#define PCONTEXT_SIZE 9
#define PSIGNAL_QUEUE_SIZE 3

// If changing any of these typedefs, update 
// libiridium/include/sys/process.h
typedef int16_t PID_T;
typedef uint32_t PPERM_T;
typedef uint64_t PSIGNAL_T;

/*
 *  Bits 31-0 is signal payload (sigpayload).
 *  Bits 47-32 is the source PID that sent the signal.
 *  Bits 63-48 are reserved (keep 0).
 *
 */

struct SignalQueue {
    uint8_t rwlock;
    PSIGNAL_T queue[PSIGNAL_QUEUE_SIZE];        // Recv queue, holds signal until process reads it.
    void* signal_callback;                      // Signal callback which gets called on a signal being sent.
    uint8_t next_index;
};


// Process structures.
typedef enum {
    PSTATE_BLOCKED = 0,
    PSTATE_RUNNING = 1,
    PSTATE_READY = 2,
} PSTATE_T;


// Process flags.
#define PFLAG_SIGNAL_HANDLER (1 << 0)               // Process signal handler (i.e process spawned to handle signal callback).

// Context.
typedef enum {
    PCTX_RBP = 0,
    PCTX_RSP = 1,
    PCTX_RBX = 2,
    PCTX_RDX = 3,
    PCTX_RCX = 4,
    PCTX_RAX = 5,
    PCTX_RIP = 6,
    PCTX_CR3 = 7,
    PCTX_STACK_BASE = 8
} PCONTEXT_INDEX;

struct Process {
    PID_T pid;                                                  // Process identifier.
    PSTATE_T state;                                             // Process state.
    uint32_t n_sleep_ticks;                                     // A counter to unblock task once it hits 0.
    PPERM_T perm_mask;                                          // Premission bitmask.
    uint64_t context[PCONTEXT_SIZE];                            // Context (registers and address space).
    uint16_t n_slave_driver_groups;                             // The amount of driver groups this process owns.
    DRIVER_TYPE_T slave_driver_groups[DRVMASTER_MAX_SLAVES];    // Driver group this process owns (used by daemons, if not used: DRIVERTYPE_NONE).
    uint8_t uses_console : 1;                                   // 1 if process should have error info output to console.
    uint16_t flags;                                             // Process flags.
    struct SignalQueue sigq;                                    // Signal queue.
    struct Process* parent;                                     // Signal queue.
    struct Process* prev;                                       // Pointer to process on the prev.
    struct Process* next;                                       // Pointer to process on the next.
};

__attribute__((naked)) void proc_init(void);

// Assembly helpers for proc.asm
uint64_t* proc_get_context(struct Process* root);
struct Process* proc_get_next(struct Process* root);
void proc_set_state(struct Process* root, PSTATE_T state);
uint64_t fetch_rip(void);

// Permission related stuff.
ERRNO_T perm_grant(PID_T pid, PPERM_T perms);
ERRNO_T perm_revoke(PID_T pid, PPERM_T perms);


// Process creation/deletion.
// May be replaced in later versions.
// @param uses_console is 1 if the process was spawned by a console and
// if the console should be used for programs error output.
PID_T _initrd_spawn(const char* path, PPERM_T permissions, uint8_t uses_console);

// Exits the current process.
void exit(void);

// For internal kernel usage if a task does something it's 
// not supposed to and causes a fault.
__attribute__((noreturn)) void handle_exception(uint8_t vector, uint64_t rip, uint64_t errcode);

// Kills a process.
__attribute__((noreturn)) void kill(PID_T pid, ERRNO_T* errno_out);

// Signal stuff.
ERRNO_T psignal_send(PID_T to, uint32_t dword);
uint64_t psignal_read(void);

#endif
