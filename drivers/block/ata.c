#include <drivers/block/ata.h>
#include <drivers/pci.h>
#include <asm/ring0.h>
#include <printk.h>
#include <cpu.h>

static char sect_buffer[512];

static void
identify_ata(unsigned short port, unsigned char dev)
{
	outb_p(port + ATA_DRIVE, dev);
	for (int i = port + ATA_SECT; i <= port + ATA_LBAHI; i++) {
		outb_p(i, 0);
	}
	outb_p(port + ATA_CMD, ATA_CMD_IDENTIFY);
	if (inb(port + ATA_STATUS) == 0) {
		printk("ata bus[%i,%i]: no drive found\n",
		       port == PRIM_CHNN_BASE ? 0 : 1,
		       dev == ATA_MASTER_DRV ? 0 : 1);
		return;
	}
	while (1) {
		// printk("%x\n", inb(port+ATA_STATUS));
		if (inb(port + ATA_STATUS) & ATA_STATUS_ERR) {
			printk("ata bus[%i,%i]: no drive found\n",
			       port == PRIM_CHNN_BASE ? 0 : 1,
			       dev == ATA_MASTER_DRV ? 0 : 1);
			return;
		}
		if ((inb(port + ATA_STATUS) & ATA_STATUS_BSY) == 0) {
			if ((inb(port + ATA_STATUS) & ATA_STATUS_REDY) ==
			    ATA_STATUS_REDY) {
				break;
			}
		}
		io_wait();
	}
	unsigned short *buf = (unsigned short *) sect_buffer;
	for (int i = 0; i < 256; i++) {
		buf[i] = inw(port + ATA_DATA);
	}
	printk("ata bus[%i,%i]: ", port == PRIM_CHNN_BASE ? 0 : 1,
	       dev == ATA_MASTER_DRV ? 0 : 1);
	unsigned int *capacity = (unsigned int *) (&buf[60]);
	printk("generic IDE harddisk (%iMB)\n", (*capacity) / 2048);
}

void
ata_init()
{
	identify_ata(PRIM_CHNN_BASE, ATA_MASTER_DRV);
	printk("%p\n", sect_buffer);
	identify_ata(PRIM_CHNN_BASE, ATA_SLAVE_DRV);
	identify_ata(SCND_CHNN_BASE, ATA_MASTER_DRV);
	identify_ata(SCND_CHNN_BASE, ATA_SLAVE_DRV);
}
