#include <cpuid.h>
#include <apic.h>
#include <acpi.h>
#include <panic.h>
#include <mm/mmap.h>
#include <printk.h>
#include <asm/ring0.h>

static unsigned int
ioapic_read(unsigned int reg)
{
	unsigned int volatile *io = (unsigned int volatile *) ioapic_addr;
	io[0] = (reg & 0xff);
	return io[4];
}

static void
ioapic_write(unsigned int reg, unsigned int data)
{
	unsigned int volatile *io = (unsigned int volatile *) ioapic_addr;
	io[0] = (reg & 0xff);
	io[4] = data;
}

static unsigned int
lapic_read(unsigned int offset)
{
	unsigned int volatile *io = (unsigned int volatile *) apic_addr;
	return io[offset >> 2];
}

static void
lapic_write(unsigned int offset, unsigned int data)
{
	unsigned int volatile *io = (unsigned int volatile *) apic_addr;
	io[offset >> 2] = data;
}

void
eoi_apic()
{
	lapic_write(0xB0, 0);
}

void
ioapic_enable(unsigned char target_vector, unsigned char pin)
{
	ioapic_write(0x10 + 2 * pin, target_vector);
	ioapic_write(0x10 + 2 * pin + 1, 0);
}

void
ioapic_disable(unsigned char pin)
{
	ioapic_write(0x10 + 2 * pin, 1 << 16 | ioapic_read(0x10 + 2 * pin));
	ioapic_write(0x10 + 2 * pin + 1, 0);
}

void
apic_init()
{
	// disable 8259 pic
	outb(PIC_1_DATA, 0xff);
	outb(PIC_2_DATA, 0xff);
	// hard enable local apic
	unsigned int eax, edx;
	rdmsr(IA32_APIC_BASE_MSR, &eax, &edx);
	eax |= IA32_APIC_BASE_MSR_ENABLE;
	wrmsr(IA32_APIC_BASE_MSR, eax, edx);
	lapic_write(LAPIC_SPR_INR_VEC,
		    lapic_read(LAPIC_SPR_INR_VEC) | LAPIC_SPR_ENABLE);
}
