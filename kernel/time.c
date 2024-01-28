#include <asm/ring0.h>
#include <time.h>
#include <pic.h>
#include <cpu.h>

static unsigned long jeffies = 0;

void pit_init(){
	set_irq_gate(0, &pit_int);
	outb(PIT_CMD, PIT_CHNN0|PIT_RW_HL|PIT_MODE_RATEGEN);
	outb(PIT_DATA0, (unsigned char)(DIVISOR&0xff));
	outb(PIT_DATA0, (unsigned char)(DIVISOR>>8));
	enable_irq(1);
}

void do_pit_int(){
	jeffies++;
	eoi_8259(0);
}