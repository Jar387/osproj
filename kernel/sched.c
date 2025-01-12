#include <cpu.h>
#include <sched.h>
#include <mm/slab.h>
#include <mm/buddy.h>
#include <printk.h>
#include <lib/string.h>
#include <asm/ring0.h>
#include <panic.h>

void init1(){
	printk("init1\n");
	int idx = 0;
	for(;;){
		lock_kernel();
		//printk("loop1 %i\n", idx);
		for(int i=0;i<1000000;i++){
			nop();
		}
		idx++;
		unlock_kernel();
	}
}

void init2(){
	printk("init2\n");
	int idx = 0;
	for(;;){
		lock_kernel();
		//printk("loop2 %i\n", idx);
		for(int i=0;i<1000000;i++){
			nop();
		}
		idx++;
		unlock_kernel();
	}
}

struct task_struct* tss_head = NULL;
struct task_struct* curr = NULL;

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
	new->time_slice = DEFAULT_TIMESLICE;
	// insert to list
	if(tss_head==NULL){
		tss_head = new;
		new->next = NULL;
	}else{
		new->next = tss_head->next;
		tss_head->next = new;
	}
	return new;
}

void sched_init(){
	new_tss(&init1);
	new_tss(&init2);
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

void find_target_tss(struct task_struct** from, struct task_struct** to){
	struct task_struct* index = tss_head;
	if(tss_head==NULL){ // init should always be alive
		panic("init(pid=1) died");
	}
	if(curr==NULL){ // handle initial state
		curr = tss_head;
		*from = NULL;
		*to = tss_head;
		return;
	}
	curr->time_slice--;
	if(curr->time_slice==0){
		// need schedule
		bp();
		curr->time_slice = DEFAULT_TIMESLICE;
		curr->status = RUNNABLE;
		*from = curr;
		curr = curr->next;
		if(curr==NULL){
			curr = tss_head;
			while(curr!=NULL){
				curr->status = RUNNABLE;
				curr->time_slice = DEFAULT_TIMESLICE;
				curr = curr->next;
			}
			curr = tss_head;
		}
		*to = curr;
		// don't set status here, maybe new
	}else{
		*from = NULL;
		*to = NULL;
	}
}

void do_sched(struct sched_stack* stack_frame){
	printk("%x\n", stack_frame->cr3);
	struct task_struct* from;
	struct task_struct* to;
	find_target_tss(&from, &to);
	if(from==NULL&&to==NULL){
		return;
	}
	if(from!=NULL){
		save_to(stack_frame, from);
	}
	if(to->status = NEW){
		switch_to_new(stack_frame, to);
	}else{
		switch_to(stack_frame, to);
	}
	to->status = RUNNING;
}