bits 64

global switch_to_ring3
extern pit_lock

switch_to_ring3:
    ;; This method of switching to ring 3 involves making
    ;; the CPU think it is already in ring 3 and just switching back.
    ;; This works by setting up an IRET stack frame manually with 
    ;; bottom bits of the segment selectors set to 3 (SELECTOR | 3) which is the 
    ;; RPL we want.
    ;; Stack should look like this:
    ;;
    ;; SS
    ;; RSP
    ;; RFLAGS
    ;; CS
    ;; RIP      <- We'll set this to the location of the init system entry point.

    mov ax, 0x40 | 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rax, rsp
    push 0x40 | 3
    push rax
    pushf
    mov rax, [rsp]
    or rax, 1 << 9
    mov [rsp], rax
    push 0x38 | 3
    push rdi
    iretq
