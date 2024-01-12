#include <asm/cpuins.h>
#include <drivers/char.h>

void kmain(){
	cwrite(0, "hello", 5);
	for(;;){
		hlt();
	}
}