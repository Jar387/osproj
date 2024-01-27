#include <asm/ring0.h>
#include <cpu.h>
#include <multiboot.h>
#include <printk.h>
#include <drivers/char.h>
#include <mm.h>

void kmain(struct multiboot_info* info){
	lock_kernel();
	cdev_preload(info);
	printk("Setting up hardwares\n");
	arch_init();
	mm_init(info->mem_upper, info->mem_lower);
	printk("hardware setup done\n");
	for(;;){
		hlt();
	}
}