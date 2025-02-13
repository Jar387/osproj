#include <asm/ring0.h>
#include <drivers/char.h>
#include <drivers/char/serial.h>
#include <kernel.h>
#include <errno.h>
#include <stddef.h>

static int
init_one_com(unsigned short port)
{
	outb(port + 1, 0x00);
	outb(port + 3, 0x80);
	outb(port + 0, 0x03);
	outb(port + 1, 0x00);
	outb(port + 3, 0x03);
	outb(port + 2, 0xC7);
	outb(port + 4, 0x0B);
	outb(port + 4, 0x1E);
	outb(port + 0, 0xAE);
	if (inb(port + 0) != 0xAE) {
		return 1;
	}
	outb(port + 4, 0x0F);
	return 0;
}

void
serial_init()
{
	for (int i = 0; (i < 4) && ((BIOS_DATA->serial_addr[i]) != 0); i++) {
		init_one_com(BIOS_DATA->serial_addr[i]);
	}
}
