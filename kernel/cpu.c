#include <asm/ring0.h>
#include <cpu.h>
#include <interrupt.h>

static struct segment_desc gdt[5];
static struct xdtr gdtr;

static struct gate_desc idt[256];
static struct xdtr idtr;

static void setup_gdt(){
	gdtr.limit = 5*sizeof(struct segment_desc);
	gdtr.addr = &gdt;
	int* ptr = (int*)gdt;
	*ptr = 0;
	ptr++;
	*ptr = 0;
	for(int i=1;i<5;i++){
		gdt[i].limitlow = 0xffff;
		gdt[i].baselow = 0;
		gdt[i].basemid = 0;
		gdt[i].sysflag = RWX;
		gdt[i].present = 1;
		gdt[i].limithigh = 0xf;
		gdt[i].magic = MAGIC;
		gdt[i].basehigh = 0;
	}
	gdt[1].type = RX;
	gdt[2].type = RW;
	gdt[3].type = RX;
	gdt[4].type = RW;
	gdt[1].dpl = RING0;
	gdt[2].dpl = RING0;
	gdt[3].dpl = RING3;
	gdt[4].dpl = RING3;
	lgdt(&gdtr);
	flush_segment(1<<3, 2<<3);
}

static void setup_idt(){
	idtr.limit = 256*sizeof(struct gate_desc);
	idtr.addr = &idt;
	lidt(&idtr);
}

void register_intr(unsigned char num, void* ISR,unsigned short segment , unsigned char dpl){
	idt[num].addrlow = (unsigned short)((unsigned int)ISR&0xffff);
	idt[num].segment = segment;
	idt[num].reserve0 = 0;
	idt[num].type = INTR_GATE;
	idt[num].dpl = dpl;
	idt[num].present = 1;
	idt[num].addrhigh = (unsigned short)(((int)ISR>>16)&0xffff);
}

void register_trap(unsigned char num, void* ISR,unsigned short segment , unsigned char dpl){
	idt[num].addrlow = (unsigned short)((unsigned int)ISR&0xffff);
	idt[num].segment = segment;
	idt[num].reserve0 = 0;
	idt[num].type = TRAP_GATE;
	idt[num].dpl = dpl;
	idt[num].present = 1;
	idt[num].addrhigh = (unsigned short)(((int)ISR>>16)&0xffff);
}

void arch_init(){
	setup_gdt();
	setup_idt();
	set_int_gate(0, &divide_error);
	set_int_gate(1, &debug);
	set_int_gate(2, &nmi);
	set_system_gate(3, &int3);
	set_system_gate(4, &overflow);
	set_system_gate(5, &bounds);
	set_int_gate(6, &opcode);
	set_int_gate(7, &device_not_present);
	set_int_gate(8, &double_fault);
	set_int_gate(9, &i387_overrun);
	set_int_gate(10, &illegal_tss);
	set_int_gate(11, &illegal_segment);
	set_int_gate(12, &stack_overflow);
	set_int_gate(13, &general_protect);
	set_int_gate(14, &page_fault);
	for(int i=15;i<=0xff;i++){
		set_int_gate(i, &reserved);
	}
}