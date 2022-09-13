bits 64
global _initrd_spawn
extern con_out_u32

%include "src/sys/asm/_syscall.inc"

;; RDI: PATH
;; RSI: PMASK
;; RDX: USES_CONSOLE
_initrd_spawn:
    syscall_begin
    mov rax, 0x3
    mov rbx, rdi
    mov rcx, rsi
    mov rdx, rdx
    int 0x80
    syscall_end

section .data
ret_value: dq 0
