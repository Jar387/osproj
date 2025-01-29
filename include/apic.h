#ifndef APIC_H
#define APIC_H

void apic_init();

void eoi_apic();

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

#endif
