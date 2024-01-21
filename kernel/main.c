#include <asm/cpuins.h>
#include <drivers/char.h>
#include <cpu.h>
#include <printk.h>

void kmain(){
	printk("Booting the kernel\n");
	arch_init();
	printk("hardware setup finish\n");
	for(;;){
		hlt();
	}
}