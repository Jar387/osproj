#include <asm/ring0.h>
#include <drivers/char.h>
#include <drivers/char/devmem.h>
#include <errno.h>
#include <stddef.h>

static char *mem_ptr;
static short port_ptr;

static int
devmem_read(short minor)
{
	char result;
	switch (minor) {
	case MINOR_DEVMEM_MEM:
		result = *mem_ptr;
		mem_ptr++;
		if (mem_ptr >= MEM_DEV_HI) {
			mem_ptr = MEM_DEV_LO;
		}
		break;
	case MINOR_DEVMEM_NULL:
	case MINOR_DEVMEM_FULL:
	case MINOR_DEVMEM_ZERO:
	default:
		result = 0;
		break;
	case MINOR_DEVMEM_PORT:
		result = inb(port_ptr);
		port_ptr++;
		if (port_ptr >= PORT_DEV_HI) {
			port_ptr = PORT_DEV_LO;
		}
		break;
	}
	return (int) result;
}

static int
devmem_write(short minor, char data)
{
	switch (minor) {
	case MINOR_DEVMEM_FULL:
		return -ENOSPC;
	default:
		return -EPERM;
	}
}

void
init_devmem()
{
	creat_cdev(MAJOR_MEM, devmem_read, devmem_write, NULL);
	mem_ptr = MEM_DEV_LO;
	port_ptr = PORT_DEV_LO;
}
