bits 64
global irq0_isr
global pit_ticks

extern lapic_send_eoi

irq0_isr:
	cli
	inc dword [pit_ticks]
	call lapic_send_eoi
	sti
	iretq

section .data
pit_ticks: dw 0
