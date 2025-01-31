#ifndef APIC_H
#define APIC_H

void apic_init();

void ioapic_enable(unsigned char target_vector, unsigned char pin);
void ioapic_disable(unsigned char pin);

void eoi_apic();

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

#define LAPIC_ID 0x20
#define LAPIC_VERSION 0x30
#define LAPIC_TPR 0x80
#define LAPIC_APR 0x90
#define LAPIC_PPR 0xa0
#define LAPIC_EOI 0xb0
#define LAPIC_RRD 0xc0
#define LAPIC_LOG_DST 0xd0
#define LAPIC_DEST_FMT 0xe0
#define LAPIC_SPR_INR_VEC 0xf0

#define LAPIC_SPR_ENABLE 0x100

#define LAPIC_ISR_HEAD 0x100
#define LAPIC_TMR_HEAD 0x180
#define LAPIC_IRR_HEAD 0x200
#define LAPIC_ERR 0x280
#define LAPIC_LVR_TMR 0x320
#define LAPIC_TMR_ICR 0x380
#define LAPIC_TMR_CCR 0x390
#define LAPIC_TMR_DCR 0x3e0

#define PIC_1_DATA 0x21
#define PIC_2_DATA 0xa1

#endif
