#ifndef SCHED_H
#define SCHED_H

#include <cpu.h>

#define RUNNABLE 0
#define RUNNING 1
#define BLOCKED 2

typedef unsigned int reg;

struct task_struct{
	unsigned int pid;
	unsigned int status;
	void* kernel_stack;
	struct tss task_state;
	struct task_struct* next;
};

struct sched_stack{
	reg ss;
	reg esp;
	reg eflags;
	reg cs;
	reg eip;
	reg eax;
	reg ebx;
	reg ecx;
	reg edx;
	reg esi;
	reg edi;
	reg ebp;
	reg ds;
	reg es;
	reg fs;
};

void do_sched(void* stack_frame);
void schedule();

#endif