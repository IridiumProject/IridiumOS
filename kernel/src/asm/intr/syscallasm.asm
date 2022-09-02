bits 64

global dispatch_syscall
extern syscall_regs
extern syscall_table
extern g_SYSCALL_COUNT

;; NOTE: Use anything other than RAX as a return value for syscalls.
%macro set_reg_at 2
    mov r11, syscall_regs
    add r11, 8*%1
    mov qword [r11], %2
%endmacro


%macro get_reg_at 1
    mov r11, syscall_regs
    add r11, 8*%1
    mov rax, qword [r11]
%endmacro


dispatch_syscall:
    cli

    movzx r11, word [g_SYSCALL_COUNT]
    cmp rax, r11
    jge .done

    cmp rax, 0
    jl .done
    
    set_reg_at 0, rax
    set_reg_at 1, rbx
    set_reg_at 2, rcx
    set_reg_at 3, rdx
    set_reg_at 4, rsi
    set_reg_at 5, rdi
    set_reg_at 6, r8
    set_reg_at 7, r9
    set_reg_at 8, r10
    
    mov r11, syscall_table
    imul rax, 8
    add r11, rax
    call [r11]    

    get_reg_at 1
    mov rbx, rax

    get_reg_at 2
    mov rcx, rax

    get_reg_at 3
    mov rdx, rax

    get_reg_at 4
    mov rsi, rax


    get_reg_at 5
    mov rdi, rax

    get_reg_at 6
    mov r8, rax

    get_reg_at 7
    mov r9, rax

    get_reg_at 8
    mov r10, rax

    get_reg_at 0

    .done:
        iretq
