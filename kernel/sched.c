#include <cpu.h>
#include <sched.h>
#include <mm/slab.h>
#include <mm/buddy.h>
#include <printk.h>

void init(){
	printk("init\n");
}

static struct task_struct init_task = {
	.pid = 1,
	.status = RUNNABLE,
	.reg.eip = (unsigned int)(&init),
	.next = NULL
};

void do_sched(void* stack_frame){
	struct sched_stack* stack = (struct sched_stack*)stack_frame;
	
}