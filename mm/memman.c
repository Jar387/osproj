#include <printk.h>

void page_init(unsigned int);

void mm_init(unsigned int memupper, unsigned int memlower){
	unsigned int total_mem = ((memupper+1024)&0xfffff800)+2048; // um, it works :)
	printk("mem total %i kb\n", total_mem);
	page_init(total_mem);
}