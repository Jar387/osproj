#include <cpu.h>
#include <sched.h>
#include <mm/slab.h>
#include <mm/buddy.h>
#include <printk.h>
#include <lib/string.h>
#include <asm/ring0.h>

void init1(){
	printk_s("init1\n");
	int idx = 0;
	for(;;){
		lock_kernel();
		printk_s("loop1 %i\n", idx);
		idx++;
		unlock_kernel();
	}
}

void init2(){
	printk_s("init2\n");
	int idx = 0;
	for(;;){
		lock_kernel();
		printk_s("loop2 %i\n", idx);
		idx++;
		unlock_kernel();
	}
}

struct task_struct* tss_head;

struct task_struct* tss_2;

unsigned int index = 2;

void* worker_stack_top;

unsigned int next_pid = 1;

struct task_struct* new_tss(void* entry){
	struct task_struct* new = (struct task_struct*)kmalloc(sizeof(*new));
	memset(new, 0, sizeof(*new));
	new->pid = next_pid;
	next_pid++;
	new->status = NEW;
	new->int_stack.cs = 2<<2;
	new->int_stack.eflags = 0x282;
	new->int_stack.eip = (reg32)entry;
	new->kernel_stack = palloc(ZONE_KERNEL, 1)+0xfff;
	new->generic_stack.cr3 = 0;
	new->generic_stack.ds = 2<<3;
	new->generic_stack.es = 2<<3;
	new->generic_stack.fs = 2<<3;
	new->generic_stack.gs = 2<<3;
	new->generic_stack.ss = 2<<3;
}

void sched_init(){
	tss_head = new_tss(&init1);
	tss_2 = new_tss(&init2);
	worker_stack_top = palloc(ZONE_KERNEL, 1)+0xfff;
}

void save_to(struct sched_stack* stack_frame, struct task_struct* tss){
	memcpy(&(tss->int_stack), esp_swap, sizeof(*esp_swap));
	memcpy(&(tss->generic_stack), stack_frame, sizeof(*stack_frame));
	tss->kernel_stack = esp_swap;
}

void switch_to(struct sched_stack* stack_frame, struct task_struct* tss){
	memcpy(esp_swap, &(tss->int_stack), sizeof(*esp_swap));
	memcpy(stack_frame, &(tss->generic_stack), sizeof(*stack_frame));
	esp_swap = tss->kernel_stack;
}

void switch_to_new(struct sched_stack* stack_frame, struct task_struct* tss){
	memcpy(stack_frame, &(tss->generic_stack), sizeof(*stack_frame));
	struct interrupt_stack* new_kernel_stack = (struct interrupt_stack*)(((char*)(tss->kernel_stack))-sizeof(tss->int_stack));
	memcpy(new_kernel_stack, &(tss->int_stack), sizeof(*new_kernel_stack));
	esp_swap = new_kernel_stack;
	tss->kernel_stack = new_kernel_stack;
}

void do_sched(struct sched_stack* stack_frame){
	struct task_struct* from;
	struct task_struct* to;
	if(index==1){
		from = tss_head;
		to = tss_2;
		index = 2;
	}else{
		from = tss_2;
		to = tss_head;
		index = 1;
	}
	if(to->status==NEW){
		to->status = RUNNING;
		if(from->status==RUNNING){
			save_to(stack_frame, from);
		}
		switch_to_new(stack_frame, to);
	}else{
		save_to(stack_frame, from);
		switch_to(stack_frame, to);
	}
}