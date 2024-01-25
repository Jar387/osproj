#include <asm/ring0.h>
#include <cpu.h>
#include <multiboot.h>
#include <printk.h>
#include <mm.h>
#include <mapping.h>

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