bits 64
global reboot

%define REBOOT_MAG 0xCA7F00D
%include "src/sys/asm/_syscall.inc"

;; RDI: REBOOT MAGIC.
reboot:
    syscall_begin
    cmp rdi, REBOOT_MAG
    jne .fail

    mov rax, 0x9            ;; sys_reboot
    int 0x80

    .fail:
        syscall_end
        retq
