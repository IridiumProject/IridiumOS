#include <intr/syscall.h>
#include <common/log.h>
#include <common/elf.h>
#include <common/asm.h>
#include <common/errno.h>
#include <common/font.h>
#include <uapi/sysreq.h>
#include <proc/drvmaster.h>
#include <proc/proc.h>
#include <fs/initrd.h>
#include <stdint.h>

// Change SYSCALL_COUNT not g_SYSCALL_COUNT.
#define SYSCALL_COUNT 9
const uint16_t g_SYSCALL_COUNT = SYSCALL_COUNT;

struct SyscallRegs {
    int64_t rax;
    int64_t rbx;
    int64_t rcx;
    int64_t rdx;
    int64_t rsi;
    int64_t rdi;
    int64_t r8;
    int64_t r9;
    int64_t r10;
} __attribute__((packed)) syscall_regs;


static void sys_hello(void) {
    kprintf(KINFO "SYS_HELLO: Hello!\n");
}

/*
 *  RBX: Service number.
 *  RCX: Request number.
 *  Returned in RAX: Status code.
 *  Returned in RBX: Possible output.
 *
 */

static void sys_req(void) {
    UAPI_OUT_T output;
    syscall_regs.rax = sysreq(syscall_regs.rbx, syscall_regs.rcx, &output);
    syscall_regs.rbx = output;
}


/*
 *  NOTE: Needs PPERM_DRVCLAIM permission.
 *
 *  Claims a driver.
 *
 *  RBX: Driver category, see docs/kernel/drivercat.txt
 *  Returned in RAX: Status code (ERRNO_T).
 *
 */

static void sys_claimdrv(void) {
    syscall_regs.rax = drv_claim(syscall_regs.rbx);
}


/*  
 *  Spawns a process from an ELF in initrd.
 *
 *  TODO: Remove this when finished with disk driver.
 *
 *  RBX: Filename.
 *  RCX: If calling task does not have PPERM_PERM leave RCX 0, otherwise permissions if needed.
 *  RDX: 1 if console should be used for error output, otherwise 0.
 *  Returned in RAX: if value is >= 1 it is the PID otherwise if < 0 it is an error code.
 *
 */

static void sys_ird_spawn(void) {
    extern struct Process* current_task;
    
    if (!(current_task->perm_mask & PPERM_INITRD)) {
        syscall_regs.rax = -EPERM;
        return;
    }

    // TODO: Error checking ASAP.
    // syscall_regs.rax = spawn(NULL, (const char*)syscall_regs.rbx, syscall_regs.rcx, NULL);
    syscall_regs.rax = _initrd_spawn((const char*)syscall_regs.rbx, syscall_regs.rcx, syscall_regs.rdx);
}


/*
 *  Sends a signal to a process.
 *
 *  RBX: Destination PID.
 *  RCX: Payload.
 *  Returned in RAX: Status code.
 *
 */

static void sys_send_psignal(void) {
    syscall_regs.rax = psignal_send(syscall_regs.rbx, syscall_regs.rcx);
}


/*
 *  Pops a signal out of the current task's
 *  signal queue.
 *
 *  Returns signal in RAX (uint64_t).
 *  RAX will be 0 if signal was rejected.
 *
 */


static void sys_read_psignal(void) {
    syscall_regs.rax = psignal_read();
}

/*
 *  Requires PPERM_CONSOLE.
 *  Outputs to the console.
 *  RBX: String.
 *  RCX: Color.
 *  Returned in RAX: Status.
 */

static void sys_con_out(void) {
    extern struct Process* current_task;
    if (!(current_task->perm_mask & PPERM_CONSOLE)) {
        syscall_regs.rax = -EPERM;
    } else {
        write_string((const char*)syscall_regs.rbx, syscall_regs.rcx);
        syscall_regs.rax = EXIT_SUCCESS;
    }
}

// Exits current process.
__attribute__((naked)) static void sys_exit(void) {
    exit();

    // We won't even reach here. This is just
    // to show that it won't return.
    while (1);
}

/*
 *  Process signal hook.
 *  Causes the function pointer selected to hook
 *  to be called when a signal is sent to the process.
 *
 *  RBX: Function ptr.
 *
 */

static void sys_psignal_hook(void) {
    extern struct Process* current_task;
    current_task->sigq.signal_callback = (void*)syscall_regs.rbx;
}


void(*syscall_table[SYSCALL_COUNT])(void) = {
    sys_hello,                          // 0x0.
    sys_req,                            // 0x1.
    sys_claimdrv,                       // 0x2.
    sys_ird_spawn,                      // 0x3.
    sys_send_psignal,                   // 0x4.
    sys_read_psignal,                   // 0x5.
    sys_con_out,                        // 0x6.
    sys_exit,                           // 0x7.
    sys_psignal_hook,                   // 0x8.
};
