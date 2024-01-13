.section .text
.global _intr_stub
.extern _int_stub
_intr_stub:
	cli
	pushal
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	movw $2<<3, %ax
	movw %ax, %ds
	call int_stub
	popw %gs
	popw %fs
	popw %es
	popw %ds
	popal
	iret