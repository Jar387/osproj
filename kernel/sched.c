#include <cpu.h>
#include <sched.h>
#include <mm/slab.h>
#include <mm/buddy.h>
#include <printk.h>
#include <lib/string.h>
#include <asm/ring0.h>

void init(){
	int i;
	for(;;){
		lock_kernel();
		i++;
		unlock_kernel();
	}
}

void init2(){
	int j;
	for(;;){
		lock_kernel();
		j++;
		unlock_kernel();
	}
}

struct task_struct* tss_head;
struct task_struct* tss_2;

struct task_struct* curr;

unsigned int is_init = 0;


void sched_init(){
	tss_head = (struct task_struct*)kmalloc(sizeof(*tss_head));
	memset(tss_head, 0, sizeof(*tss_head));
	tss_head->pid = 1;
	tss_head->status = RUNNABLE;
	init_list(&(tss_head->node));
	tss_head->reg.eip = (unsigned int)(&init);
	// allocate kernel stack(4KB)
	tss_head->reg.esp = (unsigned int)palloc(ZONE_KERNEL, 1) + 0xfff;
	tss_head->reg.cs = 2<<2;
	tss_head->reg.ds = 2<<3;
	tss_head->reg.es = 2<<3;
	tss_head->reg.fs = 2<<3;
	tss_head->reg.gs = 2<<3;
	tss_head->reg.ss = 2<<3;

	tss_2 = (struct task_struct*)kmalloc(sizeof(*tss_2));
	memset(tss_2, 0, sizeof(*tss_2));
	tss_2->pid = 2;
	tss_2->status = RUNNABLE;
	tss_2->reg.eip = (unsigned int)(&init2);
	// allocate kernel stack(4KB)
	tss_2->reg.esp = (unsigned int)palloc(ZONE_KERNEL, 1) + 0xfff;
	tss_2->reg.cs = 2<<2;
	tss_2->reg.ds = 2<<3;
	tss_2->reg.es = 2<<3;
	tss_2->reg.fs = 2<<3;
	tss_2->reg.gs = 2<<3;
	tss_2->reg.ss = 2<<3;

	curr = tss_head;
}

void save_status(struct sched_stack* stack_frame, struct sched_stack* tss){
	tss->cr3 = stack_frame->cr3;
	tss->ss = stack_frame->ss;
	tss->gs = stack_frame->gs;
	tss->fs = stack_frame->fs;
	tss->es = stack_frame->es;
	tss->ds = stack_frame->ds;
	tss->cs = stack_frame->cs;
	tss->eip = stack_frame->eip;
	tss->eflags = stack_frame->eflags;
	tss->eax = stack_frame->eax;
	tss->ebx = stack_frame->ebx;
	tss->ecx = stack_frame->ecx;
	tss->edx = stack_frame->edx;
	tss->esi = stack_frame->esi;
	tss->edi = stack_frame->edi;
	tss->ebp = stack_frame->ebp;
	tss->esp = stack_frame->esp;
}

void load_status(struct sched_stack* stack_frame, struct sched_stack* tss){
	stack_frame->cr3 = tss->cr3;
	stack_frame->ss = tss->ss;
	stack_frame->gs = tss->gs;
	stack_frame->fs = tss->fs;
	stack_frame->es = tss->es;
	stack_frame->ds = tss->ds;
	stack_frame->cs = tss->cs;
	stack_frame->eip = tss->eip;
	stack_frame->eflags = tss->eflags;
	stack_frame->eax = tss->eax;
	stack_frame->ebx = tss->ebx;
	stack_frame->ecx = tss->ecx;
	stack_frame->edx = tss->edx;
	stack_frame->esi = tss->esi;
	stack_frame->edi = tss->edi;
	stack_frame->ebp = tss->ebp;
	stack_frame->esp = tss->esp;
}

void do_sched(struct sched_stack* stack_frame){
	if(is_init==0){
		is_init = 1;
	}else{
		save_status(&(curr->reg), stack_frame);
	}
	if(curr==tss_head){
		curr = tss_2;
	}else{
		curr = tss_head;
	}
	load_status(stack_frame, &(curr->reg));
}