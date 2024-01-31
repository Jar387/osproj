#include <cpu.h>
#include <sched.h>
#include <slab.h>
#include <buddy.h>

extern void* stack_bottom;

static struct task_struct init_task = {
	.pid = 1,
	.status = RUNNING,
	.next = NULL,
	.kernel_stack = stack_bottom
};

static struct task_struct* task_list = &init_task;
static struct task_struct* tail = &init_task;

static struct task_struct* current_task = &init_task;

static unsigned int next_pid = 2;

struct task_struct* dup_task_struct(){
	struct task_struct* task = kmalloc(sizeof(struct task_struct));
	tail->next = task;
	tail = task;
	task->pid = next_pid;
	next_pid++;
	task->status = RUNNABLE;
	task->kernel_stack = palloc(ZONE_KERNEL, 1);
}