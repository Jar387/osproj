#ifndef SCHED_H
#define SCHED_H

#include <cpu.h>
#include <lib/list.h>

#define UNUSED 0
#define RUNNABLE 1
#define RUNNING 2
#define BLOCKED 3
#define NEW 4
#define YIELD 5

#define DEFAULT_TIMESLICE 10

#define MAX_TASK 256

typedef unsigned int reg32;
typedef unsigned short reg16;

extern struct interrupt_stack *esp_swap;

struct interrupt_stack {
	reg32 eip;
	reg32 cs;
	reg32 eflags;
} __attribute__ ((packed));

struct sched_stack {
	reg32 cr3;
	reg16 padding;
	reg16 ss;
	reg16 gs;
	reg16 fs;
	reg16 es;
	reg16 ds;
	reg32 ebp;
	reg32 edi;
	reg32 esi;
	reg32 edx;
	reg32 ecx;
	reg32 ebx;
	reg32 eax;
} __attribute__ ((packed));

struct task_struct {
	unsigned int pid;
	void *kernel_stack;
	struct interrupt_stack int_stack;
	struct sched_stack generic_stack;
	unsigned int status;
};

void do_sched(struct sched_stack *stack_frame);
void sched_init();
struct task_struct *new_tss(void *entry);

#endif
