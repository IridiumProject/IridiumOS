#include <proc/proc.h>
#include <common/debug.h>
#include <common/asm.h>
#include <mm/kheap.h>
#include <mm/vmm.h>
#include <stddef.h>

#define STACK_SIZE 0x1000*3

struct Process* queue_head = NULL;               // Top of queue.
struct Process* queue_base = NULL;               // Base of queue.
struct Process* current_task = NULL;             // Currently running task.

extern PML4* cur_pml4;


static uint64_t ret_rip;
static PID_T next_pid = 1;

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
    queue_base->perm_mask = PPERM_PERM | PPERM_DRVCLAIM;

    // Allocate a new address space.
    queue_base->context[PCTX_CR3] = (uint64_t)mkpml4();
    ASSERT(queue_base->context[PCTX_CR3] != 0);
    ASSERT(queue_base->perm_mask & PPERM_DRVCLAIM);

    /*
     *  Update vmm.c's "cur_pml4" variable.
     *  We are doing this before making a new stack
     *  because kmalloc() uses the cur_pml4 variable
     *  for mapping the allocated memory.
     */
    // cur_pml4 = (PML4*)queue_base->context[PCTX_CR3];

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
ERRNO_T perm_grant(PID_T pid, PPERM_T perms) { \
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
            return EXIT_FAILURE;
        } 
    }
}


PID_T spawn(void* rip, PPERM_T permissions) {
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

    // Setup vaddrsp.
    queue_head->context[PCTX_CR3] = (uint64_t)mkpml4();
    // cur_pml4 = (PML4*)queue_head->context[PCTX_CR3];

    // Temporaily change address spaces.
    __asm__ __volatile__("mov %0, %%cr3" :: "r" (queue_head->context[PCTX_CR3]));

    // Setup stack.
    queue_head->context[PCTX_RSP] = (uint64_t)kmalloc_user(STACK_SIZE) + (STACK_SIZE - 10);
    queue_head->context[PCTX_RBP] = queue_head->context[PCTX_RSP];
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
    return queue_head->pid;
}


void proc_set_state(struct Process* root, PSTATE_T state) {
    root->state = state;
}
