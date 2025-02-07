#include <printk.h>
#include <kernel.h>
#include <mm/slab.h>
#include <mm/buddy.h>
#include <mm/mmap.h>

void
mm_init()
{
	unsigned int memupper, memlower;
	memupper = multiboot_config->mem_upper;
	memlower = multiboot_config->mem_lower;
	unsigned int total_mem = ((memupper + 1024) & 0xfffff800) + 2048;	// um, it works :)
	printk("mem total %i kb\n", total_mem);
	page_init(total_mem);
	slab_init();
}
