#include <kernel.h>
#include <asm/ring0.h>
#include <cpu.h>
#include <multiboot.h>
#include <printk.h>
#include <drivers/char.h>
#include <drivers/pci.h>
#include <drivers/block.h>
#include <mm/mm.h>
#include <sched.h>
#include <time.h>
#include <fs/vfs.h>

struct multiboot_info *multiboot_config;

void
kmain(struct multiboot_info *info)
{
	lock_kernel();
	multiboot_config = info;
	load_graphic();
	printk("Setting up hardwares\n");
	arch_init();
	mm_init();
	pci_init();
	block_init();
	fs_init();
	char_init();
	pit_init();
	sched_init();
	printk("hardware setup done\n");
	unlock_kernel();
	for (;;) {
		hlt();
	}
}

void
init()
{
	printk("running init\n");
	test();
	for (;;) {
		hlt();
	}
}
