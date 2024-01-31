no_err_code:
	# handle cpu exception with no error code
	# we are in ring0 stack
	cli
	xchgl %ebx, (%esp)
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
	cli
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

hw_int:
	# handle hw intr
	cli
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
	pushl $0
	lea 44(%esp), %edx
	pushl %edx
	movl $2<<3, %edx
	movw %dx, %ds
	movw %dx, %es
	movw %dx, %fs # load kernel segment
	call *%eax
	popl %eax # pop out trash
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

.global divide_error, debug, nmi, int3, overflow, bounds
.global opcode, device_not_present, double_fault, i387_overrun, illegal_tss, illegal_segment
.global stack_overflow, general_protect, page_fault, reserved
.global pit_int

pit_int:
	pushl $do_pit_int
	jmp hw_int

divide_error:
	pushl $do_divide_error
	jmp no_err_code

debug:
	pushl $do_debug
	jmp no_err_code

nmi:
	pushl $do_nmi
	jmp no_err_code

int3:
	pushl $do_int3
	jmp no_err_code

overflow:
	pushl $do_overflow
	jmp no_err_code

bounds:
	pushl $do_bounds
	jmp no_err_code

opcode:
	pushl $do_opcode
	jmp no_err_code

device_not_present:
	pushl $do_device_not_present
	jmp no_err_code

double_fault:
	pushl $do_double_fault
	jmp err_code

i387_overrun:
	pushl $do_i387_overrun
	jmp no_err_code

illegal_tss:
	pushl $do_illegal_tss
	jmp err_code

illegal_segment:
	pushl $do_illegal_segment
	jmp err_code

stack_overflow:
	pushl $do_stack_overflow
	jmp err_code

general_protect:
	pushl $do_general_protect
	jmp err_code

page_fault:
	pushl $do_page_fault
	jmp err_code

reserved:
	pushl $do_reserved
	jmp no_err_code
