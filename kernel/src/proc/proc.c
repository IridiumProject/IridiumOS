#include <proc/proc.h>
#include <common/debug.h>
#include <common/asm.h>
#include <common/elf.h>
#include <common/string.h>
#include <common/log.h>
#include <common/panic.h>
#include <mm/kheap.h>
#include <mm/vmm.h>

#define STACK_SIZE 0x1000*3
#define WAIT_SWITCH STI; while (1);
#define SAFE_PTR_DEREF(ptr, value) \
    if (ptr != NULL) { *ptr = value; }

struct Process* queue_head = NULL;               // Top of queue.
struct Process* queue_base = NULL;               // Base of queue.
struct Process* current_task = NULL;             // Currently running task.

extern PML4* cur_pml4;

static uint64_t ret_rip;
static PID_T next_pid = 1;


static void psignal_queue_init(struct SignalQueue* sigq) {
    SEMAPHORE_WRITE_INIT(SEMAPHORE_REJECT, PSIGNAL_QUEUE_SIZE, sigq->qlock);
    sigq->rwlock = MUTEX_UNLOCKED;
    sigq->next_index = 0;
}


/*
 *  To set process at an instruction pointer set @param rip
 *  and leave @param path NULL.
 *
 */

static ERRNO_T _spawn(void* rip, const char* path, PPERM_T permissions) {
    CLI; 

    if (permissions != 0 && !(current_task->perm_mask & PPERM_PERM)) {
        return -EPERM;
    }

    struct Process* prev = queue_head;
    queue_head->next = kmalloc(sizeof(struct Process));
    queue_head = queue_head->next;
    queue_head->pid = next_pid++;
    queue_head->perm_mask = permissions;
    queue_head->state = PSTATE_READY;
    queue_head->n_slave_driver_groups = 0;
    queue_head->next = queue_base;
    queue_head->prev = prev;
    queue_head->n_sleep_ticks = 0;
    psignal_queue_init(&queue_head->sigq);

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
    queue_head->context[PCTX_RBP] = queue_head->context[PCTX_STACK_BASE] + (STACK_SIZE - 10);
    queue_head->context[PCTX_RSP] = queue_head->context[PCTX_RBP];
    queue_head->context[PCTX_RIP] = (uint64_t)rip;

    // Setup IRET stack frame for this task.
    uint64_t* stack = (uint64_t*)queue_head->context[PCTX_RSP];
    stack[4] = 0x40 | 3;
    stack[3] = queue_head->context[PCTX_RSP];
    uint64_t rflags = 0;
    __asm__ __volatile__("pushf; pop %0" : "=r" (rflags));
    rflags |= 0x200;

    stack[2] = rflags;
    stack[1] = 0x38 | 3;
    stack[0] = (uint64_t)rip;
    LOAD_CR3(old_cr3);
    return EXIT_SUCCESS;
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
    ERRNO_T errno = _spawn(NULL, path, permissions);

    if (errno < 0) {
        return errno;
    }
    
    queue_head->uses_console = uses_console;
    return queue_head->pid;
}

__attribute__((naked)) void kill(PID_T pid, ERRNO_T* errno_out) {
    CLI;
    if (pid == 1) {
        SAFE_PTR_DEREF(errno_out, -EPERM);
        return;
    }

    if (pid < 0 || pid > queue_head->pid) {
        SAFE_PTR_DEREF(errno_out, -EXIT_FAILURE);
        return;
    }

    // Locate task.
    // TODO: Make faster.
    static struct Process* current;
    current = queue_base;
    while (1) {
        current = current->next;

        // FIXME: FREE OR CLEAR AND REUSE PML4 ASAP.
        if (current->pid == pid) {
            PID_T running_pid = current_task->pid;      // PID of currently running process.

            // Clear process from queue.
            current_task = current->next;
            current->prev->next = current->next;
            current->next->prev = current->prev;

            // Free stack.
            kfree((void*)current->context[PCTX_STACK_BASE]);

            // Free actual process structure.
            kfree(current);

            // Successful!
            SAFE_PTR_DEREF(errno_out, EXIT_SUCCESS);

            // It wait successful, wait until process switches if it was current PID
            // that was killed.

            if (running_pid == pid) {
                STI;
                WAIT_SWITCH;
            }

            // No the PID wasn't of the currently running task, continue.
            STI;
            return;
        }

        if (current == queue_base) {
            // We are back at the beginning, could not find process.
            SAFE_PTR_DEREF(errno_out, -EXIT_FAILURE);
            return;
        } 
    }
}


__attribute__((noreturn)) void exit(void) {
    kill(current_task->pid, NULL);
    while (1);
}


#define EXCEPTION_PROCINFO                                                      \
    kprintf("Last instruction pointer before fault: %x\n", rip);                \
    kprintf("PID of process that caused the fault: %d\n", current_task->pid);   \


__attribute__((noreturn)) void handle_exception(uint8_t vector, uint64_t rip, uint64_t errcode) {
    CLI;

    switch (vector) {
        case 0x0:
            kprintf(PPANIC_NOCLEAR "Division error\n");
            EXCEPTION_PROCINFO;
            break;
        case 0x1:
            kprintf(PPANIC_NOCLEAR "Debug exception\n");
            EXCEPTION_PROCINFO;
            break;
        case 0x5:
            kprintf(PPANIC_NOCLEAR "Bound range exceeded\n");
            EXCEPTION_PROCINFO;
            break;
        case 0x6:
            kprintf(PPANIC_NOCLEAR "Invalid opcode\n");
            EXCEPTION_PROCINFO;
            break;
        case 0xA:
            kprintf(PPANIC_NOCLEAR "Segment error\n");
            EXCEPTION_PROCINFO;
            break;
        case 0xD:
            kprintf(PPANIC_NOCLEAR "Privilege violation\n");
            EXCEPTION_PROCINFO;
            break;
        case 0xE:
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

            REJ_SEMAPHORE_DOWN(&sigq->qlock);
            mutex_acquire(&sigq->rwlock);
            sigq->queue[sigq->next_index++] = (((uint64_t)current_task->pid << 32) | dword);
            mutex_release(&sigq->rwlock);
            return EXIT_SUCCESS;
        }

        current = current->next;

        if (current == queue_base) {
            // We are back at the beginning, could not find process.
            return -EXIT_FAILURE;
        } 
    }
}


uint64_t psignal_read(void) {
    struct SignalQueue* sigq = &current_task->sigq;
    
    if (sigq->next_index == 0) {
        return 0;
    }

    mutex_acquire(&sigq->rwlock);
    uint64_t signal = sigq->queue[--sigq->next_index];
    mutex_release(&sigq->rwlock);

    semaphore_up(&sigq->qlock);
    return PSIGNAL_TOP_BITS_UNTOUCHED(signal) ? signal : 0;
}
