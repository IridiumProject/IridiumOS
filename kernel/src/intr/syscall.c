#include <intr/syscall.h>
#include <common/log.h>
#include <uapi/sysreq.h>
#include <stdint.h>

// Change SYSCALL_COUNT not g_SYSCALL_COUNT.
#define SYSCALL_COUNT 2
const uint16_t g_SYSCALL_COUNT = SYSCALL_COUNT;

struct SyscallRegs {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
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


void(*syscall_table[SYSCALL_COUNT])(void) = {
    sys_hello,
    sys_req
};
