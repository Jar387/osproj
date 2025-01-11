#include <cpu.h>
#include <sched.h>
#include <mm/slab.h>
#include <mm/buddy.h>
#include <printk.h>
#include <lib/string.h>
#include <asm/ring0.h>

void init1(){
	lock_kernel();
	printk("init1\n");
	unlock_kernel();
	for(;;){
		lock_kernel();
		printk("loop1\n");
		unlock_kernel();
	}
}

void init2(){
	lock_kernel();
	printk("init2\n");
	unlock_kernel();
	for(;;){
		lock_kernel();
		printk("loop2");
		unlock_kernel();
	}
}

struct task_struct* tss_head;

void* worker_stack_top;

unsigned int next_pid = 2;

void sched_init(){
	tss_head = (struct task_struct*)kmalloc(sizeof(*tss_head));
	tss_head->pid = 1;
	tss_head->status = NEW;
	tss_head->int_stack.cs = 2<<2;
	tss_head->int_stack.eflags = 0x282;
	tss_head->int_stack.eip = (reg32)(&init1);
	tss_head->kernel_stack = palloc(ZONE_KERNEL, 1);
	tss_head->generic_stack.cr3 = 0;
	tss_head->generic_stack.ds = 2<<3;
	tss_head->generic_stack.es = 2<<3;
	tss_head->generic_stack.fs = 2<<3;
	tss_head->generic_stack.gs = 2<<3;
	tss_head->generic_stack.ss = 2<<3;
	worker_stack_top = palloc(ZONE_KERNEL, 1)+0xfff;
}

void switch_to(struct sched_stack* stack_frame, struct task_struct* tss){
	*esp_swap = tss->int_stack;
	*stack_frame = tss->generic_stack;
	esp_swap = tss->kernel_stack;
}

void switch_to_new(struct sched_stack* stack_frame, struct task_struct* tss){
	*stack_frame = tss->generic_stack;
	struct interrupt_stack* new_kernel_stack = (struct interrupt_stack*)(((char*)(tss->kernel_stack))-sizeof(tss->int_stack));
	*new_kernel_stack = tss->int_stack;
	esp_swap = new_kernel_stack;
}

void do_sched(struct sched_stack* stack_frame){
	if(tss_head->status==NEW){
		tss_head->status = RUNNING;
		switch_to_new(stack_frame, tss_head);
	}
	switch_to(stack_frame, tss_head);
}