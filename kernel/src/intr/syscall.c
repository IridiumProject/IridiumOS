#include <intr/syscall.h>
#include <common/log.h>
#include <common/elf.h>
#include <uapi/sysreq.h>
#include <proc/drvmaster.h>
#include <proc/proc.h>
#include <fs/initrd.h>
#include <stdint.h>

// Change SYSCALL_COUNT not g_SYSCALL_COUNT.
#define SYSCALL_COUNT 4
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
    kprintf(KINFO "SYS_HELLO: Hello! I see you pressed a key!\n");
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
 *  Returned in RAX: if value is >= 1 it is the PID otherwise if < 0 it is an error code.
 *
 */

static void sys_ird_spawn(void) {
    // TODO: Error checking ASAP.
    spawn(NULL, (const char*)syscall_regs.rbx, syscall_regs.rcx);
}


void(*syscall_table[SYSCALL_COUNT])(void) = {
    sys_hello,                          // 0x0.
    sys_req,                            // 0x1.
    sys_claimdrv,                       // 0x2.
    sys_ird_spawn,                      // 0x3.
};
