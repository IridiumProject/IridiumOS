#include <proc/proc.h>
#include <common/debug.h>
#include <common/asm.h>
#include <common/elf.h>
#include <common/string.h>
#include <common/log.h>
#include <common/panic.h>
#include <mm/kheap.h>
#include <mm/vmm.h>
#include <mm/pmm.h>

#define STACK_SIZE 0x1000*3
#define WAIT_SWITCH STI; while (1);
#define SAFE_PTR_DEREF(ptr, value) \
    if (ptr != NULL) { *ptr = value; }

struct Process* queue_head = NULL;               // Top of queue.
struct Process* queue_base = NULL;               // Base of queue.
struct Process* current_task = NULL;             // Currently running task.

static uint64_t ret_rip;
static PID_T next_pid = 1;


// tasking.asm
void taskjmp_userland(uint64_t task_rip, uint64_t task_rsp);

static void psignal_queue_init(struct SignalQueue* sigq) {
    sigq->rwlock = MUTEX_UNLOCKED;
    sigq->next_index = 0;
    sigq->signal_callback = NULL;
}


static struct Process* locate_proc(PID_T pid) {
    struct Process* p = queue_base;
    while (1) {
        if (p->pid == pid) {
            return p;
        }

        p = p->next;

        if (p == queue_base) {
            return NULL;
        }
    }
}


/*
 *  To set process at an instruction pointer set @param rip
 *  and leave @param path NULL.
 *
 */

static ERRNO_T _spawn(void* rip, const char* path, PPERM_T permissions, uint8_t uses_console) {
    CLI;
    
    if (permissions != 0 && !(current_task->perm_mask & PPERM_PERM)) {
        return -EPERM;
    }

    struct Process* prev = queue_head;
    queue_head->next = kmalloc(sizeof(struct Process));
    ASSERT(queue_head->next != NULL);
    queue_head = queue_head->next;
    queue_head->pid = next_pid++;
    queue_head->perm_mask = permissions;
    queue_head->state = PSTATE_READY;
    queue_head->n_slave_driver_groups = 0;
    queue_head->next = queue_base;
    queue_head->prev = prev;
    queue_head->n_sleep_ticks = 0;
    queue_head->flags = 0;
    queue_head->parent = NULL;
    queue_head->uses_console = uses_console;
    psignal_queue_init(&queue_head->sigq);
    ASSERT(locate_proc(queue_head->pid));

    // Setup vaddrsp.
    queue_head->context[PCTX_CR3] = (uint64_t)mkpml4();

    // Temporaily change address spaces.
    uint64_t old_cr3 = (uint64_t)vmm_get_vaddrsp();
    LOAD_CR3(queue_head->context[PCTX_CR3]);
    
    // See if we should fetch RIP from ELF.
    if (rip == NULL) {
        size_t unused;
        rip = elf_get_entry(path, &unused, queue_head->context[PCTX_CR3]);
    }
        
    // Setup stack. 
    queue_head->context[PCTX_STACK_BASE] = (uint64_t)kmalloc_user(STACK_SIZE); 
    ASSERT(queue_head->context[PCTX_STACK_BASE] != 0);
    queue_head->context[PCTX_RBP] = queue_head->context[PCTX_STACK_BASE] + (STACK_SIZE / 2);
    queue_head->context[PCTX_RSP] = queue_head->context[PCTX_RBP];
    queue_head->context[PCTX_RIP] = (uint64_t)rip;
    
    // Setup IRET stack frame for this task.
    uint64_t* stack = (uint64_t*)queue_head->context[PCTX_RSP];
    stack[0] = 0x40 | 3;
    stack[1] = queue_head->context[PCTX_RSP];
    uint64_t rflags = 0;
    __asm__ __volatile__("pushf; pop %0" : "=r" (rflags));
    rflags |= 0x200;

    stack[2] = rflags;
    stack[3] = 0x38 | 3;
    stack[4] = (uint64_t)rip;
    LOAD_CR3(old_cr3);
    return EXIT_SUCCESS;
}


static void psignal_exec_callback(struct SignalQueue* sigq, struct Process* to) {
    /*
     *  Check if signal callback is not NULL and
     *  if a signal is not being handled already.
     */

    if (sigq->signal_callback != NULL) {
        struct Process* prev = queue_head;
        // 1 because signal is being handled.
        _spawn(sigq->signal_callback, NULL, 0, 1);
        queue_head->perm_mask = to->perm_mask;
        queue_head->context[PCTX_CR3] = to->context[PCTX_CR3]; 
        queue_head->parent = to;
        queue_head->sigq = to->sigq;
        queue_head->next = queue_base;

        // Ensure the prev process's next is queue_head.
        queue_head->flags = PFLAG_SIGNAL_HANDLER;
        ASSERT(locate_proc(queue_head->pid) != NULL);
    }
}

__attribute__((naked)) void proc_init(void) {
    // Ensure this function isn't called twice.
    ASSERT(queue_head == NULL);
    ASSERT(queue_base == NULL);

    // Ensure interrupts are cleared.
    CLI;

    // Set ret_rip (return address).
    __asm__ __volatile__("mov (%%rsp), %0" : "=r" (ret_rip));

    // Allocate memory for the queue base then set the others.
    queue_base = kmalloc(sizeof(struct Process));
    queue_head = queue_base;
    current_task = queue_base;
    ASSERT(queue_base != NULL);

    // Setup queue head/base.
    queue_base->pid = next_pid++;
    queue_base->state = PSTATE_READY;
    queue_base->prev = queue_base;
    queue_base->next = queue_base;
    queue_base->context[PCTX_RBX] = 0;
    queue_base->context[PCTX_RDX] = 0;
    queue_base->context[PCTX_RCX] = 0;
    queue_base->context[PCTX_RAX] = 0;
    queue_base->context[PCTX_RIP] = ret_rip;
    queue_base->n_slave_driver_groups = 0;
    queue_base->perm_mask = PPERM_SUPER_ADMIN;
    queue_base->n_sleep_ticks = 0;
    queue_base->uses_console = 1;
    queue_base->flags = 0;
    queue_base->parent = NULL;
    psignal_queue_init(&queue_base->sigq);

    // Allocate a new address space.
    queue_base->context[PCTX_CR3] = (uint64_t)mkpml4();
    ASSERT(queue_base->context[PCTX_CR3] != 0);
    ASSERT(queue_base->perm_mask & PPERM_DRVCLAIM);

    __asm__ __volatile__("mov %0, %%cr3" :: "r" (queue_base->context[PCTX_CR3]));

    // Allocate a new stack.
    queue_base->context[PCTX_RSP] = (uint64_t)kmalloc_user(STACK_SIZE) + (STACK_SIZE - 10);
    queue_base->context[PCTX_RBP] = queue_base->context[PCTX_RSP];
    ASSERT(queue_base->context[PCTX_RSP] != 0);

    __asm__ __volatile__(
            "\
            mov %0, %%rsp; \
            mov %0, %%rbp; \
            mov %1, (%%rsp); \
            sti; \
            retq" :: "r" (queue_base->context[PCTX_RSP]), 
                     "r" (ret_rip));
}



// Assembly helpers for proc.asm
struct Process* proc_get_next(struct Process* root) { 
    return root->next;
}

uint64_t* proc_get_context(struct Process* root) {
    return &root->context[0];
}


// Permission related stuff.
ERRNO_T perm_grant(PID_T pid, PPERM_T perms) {
    // Check for PPERM_PERM.
    if (!(current_task->perm_mask & PPERM_PERM)) {
        return -EPERM;
    }

    struct Process* current = queue_base;
    while (1) {
        current = current->next;

        if (current->pid == pid) {
            // Found process with matching PID.
            // Set permissions.
            current->perm_mask |= perms;
            return EXIT_SUCCESS;
        }

        if (current == queue_base) {
            // We are back at the beginning, could not find process.
            return EXIT_FAILURE;
        } 
    }
}

ERRNO_T perm_revoke(PID_T pid, PPERM_T perms) {
    // Check for PPERM_PERM.
    if (!(current_task->perm_mask & PPERM_PERM)) {
        return -EPERM;
    }

    struct Process* current = queue_base;
    while (1) {
        current = current->next;

        if (current->pid == pid) {
            // Found process with matching PID.
            // Unset permissions.
            current->perm_mask &= ~(perms);
            return EXIT_SUCCESS;
        }

        if (current == queue_base) {
            // We are back at the beginning, could not find process.
            return -EXIT_FAILURE;
        } 
    }
}


PID_T _initrd_spawn(const char* path, PPERM_T permissions, uint8_t uses_console) {
    ERRNO_T errno = _spawn(NULL, path, permissions, uses_console);

    if (errno < 0) {
        return errno;
    }
    
    return queue_head->pid;
}


__attribute__((noreturn)) void kill(PID_T pid, ERRNO_T* errno_out) {
    CLI;

    /*
     *  We must do some checking to ensure
     *  we are putting a correct PID.
     */
    if (pid == 1) {
        SAFE_PTR_DEREF(errno_out, -EPERM);
        taskjmp_userland(current_task->context[PCTX_RIP], current_task->context[PCTX_RSP]);
    }


    if (pid < 0 || pid > queue_head->pid) {
        SAFE_PTR_DEREF(errno_out, -EXIT_FAILURE); 
        taskjmp_userland(current_task->context[PCTX_RIP], current_task->context[PCTX_RSP]);
    }

    struct Process* current = locate_proc(pid);
    if (current == NULL) {
        SAFE_PTR_DEREF(errno_out, -EXIT_FAILURE);
        taskjmp_userland(current_task->context[PCTX_RIP], current_task->context[PCTX_RSP]);
    }

    /*
     *  The is_queue_head variable
     *  is 1 if the process killed is 
     *  the queue head, otherwise 0.
     *
     */
    PID_T running_pid = current_task->pid;      // PID of currently running process. 
    if (running_pid == queue_head->pid) {
        --next_pid;
    }

    /*
     *  Once we find the process we must do
     *  the following:
     *
     *  - Free the stack allocated for it.
     *  - Free it's address space.
     *  - Remove it from the process queue.
     *  - Return to next process.
     *  
     */
            
    // Free stack.
    kfree((void*)current->context[PCTX_STACK_BASE]); 

    // Free address space.
    pmm_free(current->context[PCTX_CR3]);

    // Remove from queue.
    current->prev->next = current->next;
    current->next->prev = current->prev;
    
    /*
     *  If we are killing the head process
     *  make the head queue_head->prev.
     */
    if (pid == queue_head->pid) {
        queue_head = queue_head->prev;
    }

    // Free actual process structure.
    kfree(current);

    // Successful!
    SAFE_PTR_DEREF(errno_out, EXIT_SUCCESS); 

    // Ensure the it is indeed destroyed (for debugging).
    ASSERT(locate_proc(pid) == NULL);
    current_task = current;
    STI;
    while (1);
}


__attribute__((noreturn)) void exit(void) {
    CLI;
    kill(current_task->pid, NULL);
    while (1);
}


#define EXCEPTION_PROCINFO                                                      \
    kprintf("Last instruction pointer before fault: %x\n", rip);                \
    kprintf("PID of process that caused the fault: %d\n", current_task->pid);   \

#define CHECK_CONSOLE_USAGE \
    if (!(current_task->uses_console)) break;


__attribute__((noreturn)) void handle_exception(uint8_t vector, uint64_t rip, uint64_t errcode) {
    CLI;

    switch (vector) {
        case 0x0:
            CHECK_CONSOLE_USAGE;
            kprintf(PPANIC_NOCLEAR "Division error\n");
            EXCEPTION_PROCINFO;
            break;
        case 0x1:
            CHECK_CONSOLE_USAGE;
            kprintf(PPANIC_NOCLEAR "Debug exception\n");
            EXCEPTION_PROCINFO;
            break;
        case 0x5:
            CHECK_CONSOLE_USAGE;
            kprintf(PPANIC_NOCLEAR "Bound range exceeded\n");
            EXCEPTION_PROCINFO;
            break;
        case 0x6:
            CHECK_CONSOLE_USAGE;
            kprintf(PPANIC_NOCLEAR "Invalid opcode\n");
            EXCEPTION_PROCINFO;
            break;
        case 0xA:
            CHECK_CONSOLE_USAGE;
            kprintf(PPANIC_NOCLEAR "Segment error\n");
            EXCEPTION_PROCINFO;
            break;
        case 0xD:
            CHECK_CONSOLE_USAGE;
            kprintf(PPANIC_NOCLEAR "Privilege violation\n");
            EXCEPTION_PROCINFO;
            break;
        case 0xE:
            CHECK_CONSOLE_USAGE;
            uint64_t cr2;
            __asm__ __volatile__("mov %%cr2, %0" : "=r" (cr2));
            kprintf(PPANIC_NOCLEAR "Memory access violation\n");

            if (errcode & (1 << 1)) {
                kprintf("Violation caused by writing to address %x\n", cr2);
            } else {
                kprintf("Violation caused by reading at address %x\n", cr2);
            }
            
            EXCEPTION_PROCINFO;
            break;
    }

    if (current_task->pid == 1) {
        kprintf("\n\n");
        kprintf(KPANIC_NOCLEAR "FATAL: init.sys terminated! (System Halted)\n");
        CLI; HLT;
    }

    exit();
}


void proc_set_state(struct Process* root, PSTATE_T state) {
    root->state = state;
}


ERRNO_T psignal_send(PID_T to, uint32_t dword) { 
    struct Process* current = queue_base;

    while (1) {
        if (current->pid == to) {
            // Found process with matching PID.
            struct SignalQueue* sigq = &current->sigq;
            if (sigq->next_index >= PSIGNAL_QUEUE_SIZE) {
                return -EREJECTED;
            }
            
            if (sigq->rwlock == 1) {
                return -EBUSY;
            }

            PID_T sender_pid = current_task->flags & PFLAG_SIGNAL_HANDLER ? current_task->parent->pid : current_task->pid;
            sigq->rwlock = 1;
            sigq->queue[sigq->next_index++] = (((uint64_t)sender_pid << 32) | dword);
            psignal_exec_callback(sigq, current);
            return EXIT_SUCCESS;
        }

        current = current->next;

        if (current == queue_base) {
            kprintf("FAILED SIGNAL SENT TO: %d\n", to);
            // We are back at the beginning, could not find process.
            return -EXIT_FAILURE;
        } 
    }
}


uint64_t psignal_read(void) {
    struct SignalQueue* sigq = &current_task->sigq;
    
    /*
     *  If current task is a signal handler
     *  unlock the parent's rwlock, otherwise unlock 
     *  the current task's rwlock.
     */

    if (current_task->flags & PFLAG_SIGNAL_HANDLER) {
        current_task->parent->sigq.rwlock = 0;
    } else {
        sigq->rwlock = 0;
    }

    // No more signals to read.
    if (sigq->next_index == 0) { 
        return 0;
    }
    
    
    uint64_t signal;
    if (!(current_task->flags & PFLAG_SIGNAL_HANDLER)) {
        signal = sigq->queue[--sigq->next_index];
    } else {
        sigq = &current_task->parent->sigq;
        signal = sigq->queue[--sigq->next_index];
    }

    return PSIGNAL_TOP_BITS_UNTOUCHED(signal) ? signal : 0;
}
