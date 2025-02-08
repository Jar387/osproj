#include <kernel.h>
#include <asm/ring0.h>
#include <cpu.h>
#include <multiboot.h>
#include <printk.h>
#include <drivers/char.h>
#include <drivers/block.h>
#include <mm/mm.h>
#include <sched.h>
#include <time.h>
#include <fs/vfs.h>

struct multiboot_info *multiboot_config;

void
kmain(struct multiboot_info *info)
{
	lock_kernel();
	multiboot_config = info;
	load_graphic();
	printk("Setting up hardwares\n");
	arch_init();
	mm_init();
	blkdev_load();
	fs_init();
	cdev_load();
	pit_init();
	sched_init();
	printk("hardware setup done\n");
	unlock_kernel();
	for (;;) {
		hlt();
	}
}

void
init()
{
	printk("running init\n");
	printk
	    ("\033[31mc\033[32mo\033[33ml\033[1;33mo\033[35mr\033[36mf\033[37mu\033[1;31ml\n");
	for (;;) {
		hlt();
	}
}
