#include <printk.h>

void panic(const char* reason){
	printk("Kernel Panic: %s\n", reason);
}

void int_stub(){

}