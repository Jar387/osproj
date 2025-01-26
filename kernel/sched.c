#include <cpu.h>
#include <sched.h>
#include <mm/slab.h>
#include <mm/buddy.h>
#include <printk.h>
#include <lib/string.h>
#include <asm/ring0.h>
#include <panic.h>
#include <kernel.h>

static task_struct_t tss_list[MAX_TASK];
static unsigned int used_index = 0;
static unsigned int curr_index = 0;

void *worker_stack_top;

static unsigned int next_pid = 1;

struct task_struct *
new_tss(void *entry)
{
	if (used_index > MAX_TASK - 1) {
		panic("tss full");
	}
	task_struct_t *new = tss_list + used_index;
	memset(new, 0, sizeof (*new));
	new->pid = next_pid;
	next_pid++;
	new->status = NEW;
	new->int_stack.cs = 2 << 2;
	new->int_stack.eflags = 0x282;
	new->int_stack.eip = (reg32) entry;
	new->kernel_stack = palloc(ZONE_KERNEL, 1) + 0xfff;
	new->generic_stack.cr3 = 0;
	new->generic_stack.ds = 2 << 3;
	new->generic_stack.es = 2 << 3;
	new->generic_stack.fs = 2 << 3;
	new->generic_stack.gs = 2 << 3;
	new->generic_stack.ss = 2 << 3;
	used_index++;
	return new;
}

void
sched_init()
{
	for (int i = 0; i < MAX_TASK; i++) {
		memset(tss_list + i, 0, sizeof (task_struct_t));
	}
	new_tss(init);
	worker_stack_top = palloc(ZONE_KERNEL, 1) + 0xfff;
}

static void
save_to(sched_stack_t * stack_frame, task_struct_t * tss)
{
	memcpy(&(tss->int_stack), esp_swap, sizeof (*esp_swap));
	memcpy(&(tss->generic_stack), stack_frame, sizeof (*stack_frame));
	tss->kernel_stack = esp_swap;
}

static void
switch_to(sched_stack_t * stack_frame, task_struct_t * tss)
{
	memcpy(esp_swap, &(tss->int_stack), sizeof (*esp_swap));
	memcpy(stack_frame, &(tss->generic_stack), sizeof (*stack_frame));
	esp_swap = tss->kernel_stack;
}

static void
switch_to_new(sched_stack_t * stack_frame, task_struct_t * tss)
{
	memcpy(stack_frame, &(tss->generic_stack), sizeof (*stack_frame));
	interrupt_stack_t *new_kernel_stack =
	    (struct interrupt_stack *) (((char *) (tss->kernel_stack)) -
					sizeof (tss->int_stack));
	memcpy(new_kernel_stack, &(tss->int_stack), sizeof (*new_kernel_stack));
	esp_swap = new_kernel_stack;
	tss->kernel_stack = new_kernel_stack;
}

void
do_sched(sched_stack_t * stack_frame)
{
	unsigned int last_index = curr_index;
	curr_index++;
	if ((curr_index > (MAX_TASK - 1)) || (tss_list[curr_index].pid == 0)) {
		curr_index = 0;
	}
	if (tss_list[last_index].status != NEW) {
		save_to(stack_frame, tss_list + last_index);
		tss_list[last_index].status = RUNNABLE;
	}
	if (tss_list[curr_index].status == NEW) {
		switch_to_new(stack_frame, tss_list + curr_index);
	}
	if (tss_list[curr_index].status == RUNNABLE) {
		switch_to(stack_frame, tss_list + curr_index);
	}
	tss_list[curr_index].status = RUNNING;
}
