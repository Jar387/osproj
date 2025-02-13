#include <asm/ring0.h>
#include <drivers/char.h>
#include <drivers/char/serial.h>
#include <kernel.h>
#include <errno.h>
#include <stddef.h>

static unsigned short ports[4];

static int
init_one_com(unsigned short port, int index)
{
	outb(SERIAL_INT(port), 0x00);
	outb(SERIAL_LINECTL(port), 0x80);
	outb(SERIAL_DATA(port), 0x03);
	outb(SERIAL_INT(port), 0x00);
	outb(SERIAL_LINECTL(port), 0x03);
	outb(SERIAL_FIFO(port), 0xC7);
	outb(SERIAL_MODEM(port), 0x0B);
	outb(SERIAL_MODEM(port), 0x1E);
	outb(SERIAL_DATA(port), 0xAE);
	if (inb(SERIAL_DATA(port)) != 0xAE) {
		return 1;
	}
	outb(SERIAL_MODEM(port), 0x0F);
	ports[index] = port;
	return 0;
}

static int
serial_read(short minor)
{
	if (ports[minor - 1] == 0) {
		return -ENODEV;
	}
	unsigned short port = ports[minor - 1];

}

void
serial_init()
{
	for (int i = 0; (i < 4) && ((BIOS_DATA->serial_addr[i]) != 0); i++) {
		init_one_com(BIOS_DATA->serial_addr[i], i);
	}
}
