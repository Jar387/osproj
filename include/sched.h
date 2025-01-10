#ifndef SCHED_H
#define SCHED_H

#include <cpu.h>
#include <lib/list.h>

#define RUNNABLE 0
#define RUNNING 1
#define BLOCKED 2

typedef unsigned int reg32;
typedef unsigned short reg16;

struct sched_stack{
	reg32 cr3;
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
	reg32 eip;
	reg32 cs;
	reg32 eflags;
	reg32 esp;
	reg32 ss;
}__attribute__((packed));

struct task_struct{
	unsigned int pid;
	unsigned int status;
	struct sched_stack reg;
	struct list_node node;
};

void do_sched(struct sched_stack* stack_frame);
void sched_init();

#endif