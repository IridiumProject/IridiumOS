bits 64
global switch_task
global taskjmp_userland
global fetch_rip

extern current_task
extern proc_get_next
extern proc_get_context
extern proc_set_state

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
    
    ;; Set state as READY.
    mov rdi, [current_task]
    mov rsi, 2                          ;; 2 = PSTATE_READY.
    call proc_set_state

    ;; Get next task.
    mov rdi, [current_task]
    call proc_get_next
    mov [current_task], rax
    
    ;; Set state as RUNNING.
    mov rdi, [current_task]
    mov rsi, 1                          ;; 1 = PSTATE_RUNNING.
    call proc_set_state

    ;; Get context.
    mov rdi, [current_task]
    call proc_get_context
    mov r10, rax


    ;; Restore state.
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
    mov [tmpbuf1], rax

    index_context r10, 5
    
    mov r10, [tmpbuf1]
    mov [rsp], r10
    iretq


;; Returns to a ring 3 task.
;; RDI: Task RSP from task context.
;; ONLY CALL LIKE SO:
;; taskjmp_userland(task->context[PCTX_RSP])
taskjmp_userland:
    cli
    mov rax, rdi
    mov rdi, [rdi]
    mov [tmpbuf1], rdi
    push 0x40 | 3               ;; SS.
    push rax                    ;; RSP.
    pushf                       ;; RFLAGS.
    mov rax, [rsp]
    or rax, 1 << 9
    mov [rsp], rax
    push 0x38 | 3               ;; CS.
    push qword [tmpbuf1]        ;; RIP.
    iretq


fetch_rip:
    pop rax
    jmp rax

section .data
tmpbuf: dq 0
tmpbuf1: dq 0
