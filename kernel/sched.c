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
		printk_s("loop1 %i\n", idx);
		for(int i=0;i<100000000;i++){
			nop();
		}
		idx++;
	}
}

void init2(){
	printk_s("init2\n");
	for(;;){
		printk_s("loop2");
	}
}

struct task_struct* tss_head;

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
}

void do_sched(struct sched_stack* stack_frame){
	if(tss_head->status==NEW){
		tss_head->status = RUNNING;
		switch_to_new(stack_frame, tss_head);
	}else{
		save_to(stack_frame, tss_head);
		switch_to(stack_frame, tss_head);
	}
}