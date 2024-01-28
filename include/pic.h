#ifndef PIC_H
#define PIC_H

#define PIC1_CMD 0x20
#define PIC1_DATA PIC1_CMD+1
#define PIC2_CMD 0xa0
#define PIC2_DATA PIC2_CMD+1

#define PIC_CMD_EOI 0x20

#define ICW1_ICW4 0x01
#define ICW1_SINGLE	0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10
 
#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10

#define PIC1_INTN 0x20
#define PIC2_INTN 0x28

void init_8259();
void eoi_8259(unsigned char irq);
void enable_irq(unsigned char irq);
void disable_irq(unsigned char irq);

#endif