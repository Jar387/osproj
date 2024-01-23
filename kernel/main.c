#include <asm/cpuins.h>
#include <drivers/char.h>
#include <cpu.h>
#include <printk.h>

void kmain(struct multiboot_info* info){
	lock_kernel();
	printk("Setting up hardwares\n");
	arch_init();
	mm_init(info->mem_upper, info->mem_lower);
	printk("hardware setup done\n");
	for(;;){
		hlt();
	}
}