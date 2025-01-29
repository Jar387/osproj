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
}

void
do_pit_int(void *stack_frame)
{
    jeffies++;
	do_sched(stack_frame);
    eoi_apic();
}
