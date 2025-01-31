#include <drivers/block/ata.h>
#include <asm/ring0.h>
#include <printk.h>
#include <cpu.h>

static char sect_buffer[512];

// channel must be 0 or 1
// OTHERWISE YOU ARE FUCKING VGA CARD!
static inline void
reset_ata(unsigned short port)
{
	unsigned char ctrl = inb(port);
	ctrl |= (1 << 2);
	outb_p(port, ctrl);
	ctrl &= ~(1 << 2);
	outb(port, ctrl);
}

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
	//while(inb(port+ATA_STATUS)&ATA_STATUS_BSY!=0){io_wait();}
	while (!(inb(port + ATA_STATUS) & ATA_STATUS_COMPLETE)) {
		if (inb(port + ATA_STATUS) & ATA_STATUS_ERR) {
			printk("ata bus[%i,%i]: no drive found\n",
			       port == PRIM_CHNN_BASE ? 0 : 1,
			       dev == ATA_MASTER_DRV ? 0 : 1);
			return;
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
	reset_ata(PRIM_CHNN_CTRL);
	reset_ata(SCND_CHNN_CTRL);
	identify_ata(PRIM_CHNN_BASE, ATA_MASTER_DRV);
	identify_ata(PRIM_CHNN_BASE, ATA_SLAVE_DRV);
	identify_ata(SCND_CHNN_BASE, ATA_MASTER_DRV);
	identify_ata(SCND_CHNN_BASE, ATA_SLAVE_DRV);
}
