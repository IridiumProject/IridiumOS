bits 64
global switch_task

extern current_task
extern proc_get_next
extern proc_get_context

;; Usage:
;; index_context contextptr index 
;; RAX -> context entry.
%macro index_context 2
    mov r11, %1
    add r11, 8*%2
    mov rax, [r11]
%endmacro

;; Usage:
;; set_context_index contextptr index value
%macro set_context_index 3
    mov r11, %1
    add r11, 8*%2
    mov [r11], %3
%endmacro



switch_task:
    ;; RAX saved in tmpbuf.
    mov [tmpbuf], rax

    ;; RIP saved in tmpbuf1.
    mov rax, [rsp]
    mov [tmpbuf1], rax

    mov rdi, [current_task]
    call proc_get_context

    ;; Save current state.
    set_context_index rax, 0, rbp
    set_context_index rax, 1, rsp
    set_context_index rax, 2, rbx
    set_context_index rax, 3, rdx
    set_context_index rax, 4, rcx
    mov r10, [tmpbuf]
    set_context_index rax, 5, r10
    mov r10, [tmpbuf1]
    set_context_index rax, 6, r10
    mov r10, cr3
    set_context_index rax, 7, r10

    ;; Get next task.
    mov rdi, [current_task]
    call proc_get_next
    mov [current_task], rax

    mov rdi, [current_task]
    call proc_get_context
    mov r10, rax

    index_context r10, 7
    mov cr3, rax

    index_context r10, 0
    mov rbp, rax

    index_context r10, 1
    mov rsp, rax

    index_context r10, 2
    mov rbx, rax

    index_context r10, 3
    mov rdx, rax

    index_context r10, 4
    mov rcx, rax

    index_context r10, 6
    mov [rsp], rax

    index_context r10, 5

    times 5 pop rax
    push 0x40 | 3
    push rbp
    pushf

    ;; Enable interrupts.
    mov rax, [rsp]
    or rax, 1 << 9
    mov [rsp], rax

    push 0x38 | 3
    push qword [tmpbuf1]
    iretq

section .data
tmpbuf: dq 0
tmpbuf1: dq 0
