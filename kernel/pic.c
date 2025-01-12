#include <pic.h>
#include <asm/ring0.h>

void
init_8259()
{
	outb_p(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
	outb_p(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
	outb_p(PIC1_DATA, PIC1_INTN);
	outb_p(PIC2_DATA, PIC2_INTN);
	outb_p(PIC1_DATA, 4);
	outb_p(PIC2_DATA, 2);
	outb_p(PIC1_DATA, ICW4_8086);
	outb_p(PIC2_DATA, ICW4_8086);
	// disable all interrupts       
	outb(PIC1_DATA, 0xfb);
	outb(PIC2_DATA, 0xff);
}

void
eoi_8259(unsigned char irq)
{
	if (irq > 7) {
		outb(PIC2_CMD, PIC_CMD_EOI);
	}
	outb(PIC1_CMD, PIC_CMD_EOI);
}

void
enable_irq(unsigned char irq)
{
	unsigned short port;
	unsigned char value;

	if (irq < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq -= 8;
	}
	value = inb(port) & ~(1 << irq);
	outb(port, value);
}

void
disable_irq(unsigned char irq)
{
	unsigned short port;
	unsigned char value;

	if (irq < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		irq -= 8;
	}
	value = inb(port) | (1 << irq);
	outb(port, value);
}
