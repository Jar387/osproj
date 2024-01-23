.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.set PAGE_TABLE_ITEM_SIZE, 1
.set PAGE_TABLE_ITEM_COUNT, 1024
.set USER_COUNT, 768
.set KRNL_COUNT, 256

.section .multiboot.data, "aw"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss, "aw", @nobits
stack_bottom:
.skip 4096
stack_top:

.section multiboot.text, "a"
.global _start
.type _start, @function
_start:
	cli # we don't want any ints here
	# setup paging
	# setup kernel page table at phy:0x0
	# map 0x0-0x3fffffff --> 0xc0000000-0xffffffff
	mov $stack_top, %esp
	mov $0, %edi
	movw $0x83, (%edi) # mapping hack
	addl $2, %edi
	movw $0, (%edi)
	addl $2, %edi
	xor %eax, %eax
	mov $USER_COUNT*PAGE_TABLE_ITEM_SIZE, %ecx
	cld
	rep 
	stosl
	subl $4, %edi
creatloop:
	movw $0x83, (%edi)
	addl $2, %edi
	movw %ax, (%edi)
	addw $0x40, %ax
	addl $2, %edi
	cmpl $0x1000, %edi
	jne creatloop

	# configure cpu
	xorl %eax, %eax
	movl %eax, %cr3
	movl %cr4, %eax
	orl $0x10, %eax # PSE enable
	movl %eax, %cr4
	movl %cr0, %eax
	orl $0x80000000, %eax # PG enable
	movl %eax, %cr0
	lea 2f, %ecx
	jmp *%ecx

.section .text
2:	
	# destory page hack
	xor %edi, %edi
	movl $0, (%edi)
	movl %cr3, %eax
	movl %eax, %cr3
	mov $stack_top, %esp

	addl $0xc0000000, %ebx
	pushl %ebx
	call kmain
	cli
1:	hlt
	jmp 1b
