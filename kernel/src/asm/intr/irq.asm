bits 64
global irq0_isr
global pit_ticks

extern lapic_send_eoi
extern switch_task

irq0_isr:
	cli
	inc word [pit_ticks]
	call lapic_send_eoi
    jmp switch_task
    ;; --- NO RETURN ---

section .data
pit_ticks: dw 0
