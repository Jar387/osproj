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
    outb(PIC_1_DATA, 0xff);
    outb(PIC_2_DATA, 0xff);
    // hard enable local apic
    unsigned int eax, edx;
    rdmsr(IA32_APIC_BASE_MSR, &eax, &edx);
    eax|=IA32_APIC_BASE_MSR_ENABLE;
    wrmsr(IA32_APIC_BASE_MSR, eax, edx);
    lapic_write(LAPIC_SPR_INR_VEC, lapic_read(LAPIC_SPR_INR_VEC)|LAPIC_SPR_ENABLE);
    // init apic timer
    // figure out length of 1ms
    lapic_write(LAPIC_TMR_DCR, 0x3);
    lapic_write(LAPIC_TMR_ICR, 0xffffffff);

    outb_p(0x43, 0x20);
    outb_p(0x40, 0x2e);

    lapic_write(LAPIC_TMR_ICR, 0xffffffff);

    while(inb(0x40)){
    }
    unsigned int offset = 0xffffffff-lapic_read(LAPIC_TMR_CCR);
    lapic_write(LAPIC_TMR_DCR, 0x3);
    lapic_write(LAPIC_TMR_ICR, offset);
    lapic_write(LAPIC_LVR_TMR, 32 | 0x20000);
}
