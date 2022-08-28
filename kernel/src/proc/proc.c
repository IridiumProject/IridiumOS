#include <proc/proc.h>
#include <common/debug.h>
#include <common/asm.h>
#include <mm/kheap.h>
#include <mm/vmm.h>

#define STACK_SIZE 0x1000*3


struct Process* queue_head = NULL;               // Top of queue.
struct Process* queue_base = NULL;               // Base of queue.
struct Process* current_task = NULL;             // Currently running task.

extern PML4* cur_pml4;


static uint64_t ret_rip;

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
    queue_base->pid = 1;
    queue_base->state = PSTATE_READY;
    queue_base->prev = queue_base;
    queue_base->next = queue_base;
    queue_base->context[PCTX_RBX] = 0;
    queue_base->context[PCTX_RDX] = 0;
    queue_base->context[PCTX_RCX] = 0;
    queue_base->context[PCTX_RAX] = 0;
    queue_base->context[PCTX_RIP] = ret_rip;

    // Allocate a new address space.
    queue_base->context[PCTX_CR3] = (uint64_t)mkpml4();
    ASSERT(queue_base->context[PCTX_CR3] != 0);

    /*
     *  Update vmm.c's "cur_pml4" variable.
     *  We are doing this before making a new stack
     *  because kmalloc() uses the cur_pml4 variable
     *  for mapping the allocated memory.
     */
    cur_pml4 = (PML4*)queue_base->context[PCTX_CR3];

    __asm__ __volatile__("mov %0, %%cr3" :: "r" (cur_pml4));

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
