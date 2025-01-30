#include <asm/ring0.h>
#include <time.h>
#include <cpu.h>
#include <sched.h>
#include <printk.h>
#include <apic.h>

static unsigned long jeffies = 0;

void
pit_init()
{
	set_irq_gate(PIT_IRQ, &pit_int);
	outb(PIT_CMD, PIT_CHNN0 | PIT_RW_HL | PIT_MODE_RATEGEN);
	outb(PIT_DATA0, (unsigned char) (DIVISOR & 0xff));
	outb(PIT_DATA0, (unsigned char) ((DIVISOR >> 8) & 0xff));
}

void
do_pit_int(void *stack_frame)
{
	jeffies++;
	do_sched(stack_frame);
	eoi_apic();
}
