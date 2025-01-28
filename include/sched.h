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

typedef struct {
	reg32 eip;
	reg32 cs;
	reg32 eflags;
} __attribute__ ((packed)) interrupt_stack_t;

extern interrupt_stack_t *esp_swap;

typedef struct {
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
} __attribute__ ((packed)) sched_stack_t;

typedef struct {
	unsigned int pid;
	void *kernel_stack;
	interrupt_stack_t int_stack;
	sched_stack_t generic_stack;
	unsigned int status;
} task_struct_t;

void do_sched(sched_stack_t * stack_frame);
void sched_init();
task_struct_t *new_tss(void *entry);

#endif
