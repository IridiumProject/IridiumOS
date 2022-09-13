extern mutex_acquire
extern mutex_release

section .data
_lock: db 0
_rsp: dq 0

%macro syscall_begin 0
    mov [_rsp], rsp
%endmacro


%macro syscall_end 0
    mov rsp, [_rsp]
%endmacro

section .text
