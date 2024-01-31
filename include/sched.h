#ifndef SCHED_H
#define SCHED_H

#include <cpu.h>

#define RUNNABLE 0
#define RUNNING 1
#define BLOCKED 2

struct task_struct{
	unsigned int pid;
	unsigned int status;
	void* kernel_stack;
	struct tss task_state;
	struct task_struct* next;
};


#endif