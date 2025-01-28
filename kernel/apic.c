#include <cpuid.h>
#include <apic.h>
#include <acpi.h>
#include <panic.h>
#include <mm/mmap.h>
#include <printk.h>

void
apic_init()
{
	int volatile *io = phy2lin(ioapic_addr);
	io[0] = 0x12;
	printk("%x\n", io[4]);
}
