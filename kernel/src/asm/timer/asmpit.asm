bits 64
global pit_sleep

extern pit_ticks

pit_sleep:
	mov rcx, rdi
	mov dword [e_ticks], 0
	add [e_ticks], rcx
	sti

	.sleep:
		movzx rax, word [pit_ticks]
		movzx rdx, word [e_ticks]
		cmp rax, rdx
		jge .done
		jmp .sleep
	
	.done:
		retq

section .data
e_ticks: dd 0
