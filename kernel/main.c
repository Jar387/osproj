#include <asm/cpuins.h>
#include <drivers/char.h>
#include <cpu.h>

void kmain(){
	arch_init();
	for(;;){
		hlt();
	}
}