.section .data
.global esp_swap
esp_swap:
.long 0

.section .text
no_err_code:
	# handle cpu exception with no error code
	# we are in ring0 stack
	xchgl %eax, (%esp)
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi
	pushl %ebp
	pushw %ds
	pushw %es
	pushw %fs
	pushl $0 # fake err code for c func
	lea 44(%esp), %edx
	pushl %edx
	movl $2<<3, %edx
	movw %dx, %ds
	movw %dx, %es
	movw %dx, %fs # load kernel segment
	call *%eax
	popl %eax # pop out trash
	popl %eax # pop out trash
	popw %fs
	popw %es
	popw %ds
	popl %ebp
	popl %edi
	popl %esi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret

err_code:
	xchgl %eax,4(%esp)
	xchgl %ebx,(%esp)
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi
	pushl %ebp
	pushw %ds
	pushw %es
	pushw %fs
	pushl %eax
	lea 44(%esp),%eax
	pushl %eax
	movl $2<<3,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%fs
	call *%ebx
	popl %eax
	popl %eax
	popw %fs
	popw %es
	popw %ds
	popl %ebp
	popl %edi
	popl %esi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret

sched_int:
	# handle hw intr (only for sched)
	# esp @ kernel stack top
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi
	pushl %ebp
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	pushw %ss
	pushw $0
	movl %cr3, %esi
	pushl %esi

	movl %esp, %esi
	pushl %esi
	movl $2<<3, %edx
	movw %dx, %ds
	movw %dx, %es
	movw %dx, %fs # load kernel segment
	movl $do_pit_int, %eax
	call *%eax

	addl $4, %esp # stack balance

	popl %esi # cr3
	movl %esi, %cr3

	popw %si

	popw %ss
	popw %gs
	popw %fs
	popw %es
	popw %ds
	popl %ebp
	popl %edi
	popl %esi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax

	movl (esp_swap), %esp
	iret


.global divide_error, debug, nmi, int3, overflow, bounds
.global opcode, device_not_present, double_fault, i387_overrun, illegal_tss, illegal_segment
.global stack_overflow, general_protect, page_fault, reserved
.global pit_int, ata_prim_int, ata_scnd_int

pit_int:
	cli
	movl %esp, (esp_swap)
	movl (worker_stack_top), %esp # switch to scheduler working stack
	jmp sched_int

ata_prim_int:
	cli
	pushl $do_ata_int
	hlt

ata_scnd_int:
	cli
	pushl $do_ata_int
	hlt

divide_error:
	cli
	pushl $do_divide_error
	jmp no_err_code

debug:
	cli
	pushl $do_debug
	jmp no_err_code

nmi:
	cli
	pushl $do_nmi
	jmp no_err_code

int3:
	cli
	pushl $do_int3
	jmp no_err_code

overflow:
	cli
	pushl $do_overflow
	jmp no_err_code

bounds:
	cli
	pushl $do_bounds
	jmp no_err_code

opcode:
	cli
	pushl $do_opcode
	jmp no_err_code

device_not_present:
	cli
	pushl $do_device_not_present
	jmp no_err_code

double_fault:
	cli
	pushl $do_double_fault
	jmp err_code

i387_overrun:
	cli
	pushl $do_i387_overrun
	jmp no_err_code

illegal_tss:
	cli
	pushl $do_illegal_tss
	jmp err_code

illegal_segment:
	cli
	pushl $do_illegal_segment
	jmp err_code

stack_overflow:
	cli
	pushl $do_stack_overflow
	jmp err_code

general_protect:
	cli
	pushl $do_general_protect
	jmp err_code

page_fault:
	cli
	pushl $do_page_fault
	jmp err_code

reserved:
	cli
	pushl $do_reserved
	jmp no_err_code
