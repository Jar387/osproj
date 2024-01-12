#include <asm/cpuins.h>
#include <drivers/char.h>
#include <cpu.h>

void kmain(){
	cwrite(0, "hello", 5);
	arch_init();
	for(;;){
		hlt();
	}
}