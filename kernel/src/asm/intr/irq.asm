bits 64
global irq0_isr

extern lapic_send_eoi

irq0_isr:
	cli
	call lapic_send_eoi
	sti
	iretq
