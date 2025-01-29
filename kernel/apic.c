#include <cpuid.h>
#include <apic.h>
#include <acpi.h>
#include <panic.h>
#include <mm/mmap.h>
#include <printk.h>
#include <asm/ring0.h>

static unsigned int ioapic_read(unsigned int reg){
    unsigned int volatile * io = (unsigned int volatile*)ioapic_addr;
    io[0] = (reg&0xff);
    return io[4];
}

static void ioapic_write(unsigned int reg, unsigned int data){
    unsigned int volatile * io = (unsigned int volatile*)ioapic_addr;
    io[0] = (reg&0xff);
    io[4] = data;
}

static unsigned int lapic_read(unsigned int offset){
    unsigned int volatile * io = (unsigned int volatile*)apic_addr;
    return io[offset>>2];
}

static void lapic_write(unsigned int offset, unsigned int data){
    unsigned int volatile * io = (unsigned int volatile*)apic_addr;
    io[offset>>2] = data;
}

void eoi_apic(){
    lapic_write(0xB0, 0);
}

void
apic_init()
{
    // disable 8259 pic
    outb(0x21, 0xff);
    outb(0xa1, 0xff);
    // hard enable local apic
    unsigned int eax, edx;
    rdmsr(IA32_APIC_BASE_MSR, &eax, &edx);
    eax|=IA32_APIC_BASE_MSR_ENABLE;
    wrmsr(IA32_APIC_BASE_MSR, eax, edx);
    lapic_write(0xF0, lapic_read(0xF0)|0x100);
    // timer test
    lapic_write(0x3E0, 3);
    lapic_write(0x380, 0x100);
    lapic_write(0x320, 32 | 0x20000);
}
