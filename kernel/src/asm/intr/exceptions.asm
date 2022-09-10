bits 64

extern kprintf
extern print_panic_msg
extern handle_exception


global divide_error
global debug_exception
global overflow
global bound_range_exceeded
global invalid_opcode
global no_mathcoprocessor
global double_fault
global invalid_tss
global segment_not_present
global stack_segment_fault
global general_protection_fault
global page_fault

%macro kpanic 1
    cli
    call print_panic_msg
    mov rdi, panic_msg
    mov rsi, %1
    call kprintf
    hlt
%endmacro

%macro program_panic 1
    mov rdi, %1
    pop rsi
    call handle_exception
%endmacro

section .text
divide_error:
    xor rdx, rdx                ;; NO ERROR CODE.
    program_panic 0x0

debug_exception:
    xor rdx, rdx                ;; NO ERROR CODE.
    program_panic 0x1

overflow:
    kpanic 0x4

bound_range_exceeded:
    xor rdx, rdx                ;; NO ERROR CODE.
    program_panic 0x5

invalid_opcode:
    xor rdx, rdx                ;; NO ERROR CODE.
    program_panic 0x6

no_mathcoprocessor:
    kpanic 0x7

double_fault:
    kpanic 0x8

invalid_tss:
    pop rdx                 ;; ERROR CODE.
    program_panic 0xA

segment_not_present:
    kpanic 0xB

stack_segment_fault:
    kpanic 0xC

general_protection_fault:
    pop rdx                 ;; ERROR CODE.
    program_panic 0xD

page_fault:
    pop rdx                 ;; ERROR CODE.
    program_panic 0xE


section .data
panic_msg: db "VECTOR FIRED: %x", 0xA, 0
