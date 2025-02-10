#include <drivers/block/ata.h>
#include <drivers/pci.h>
#include <asm/ring0.h>
#include <printk.h>
#include <cpu.h>
#include <apic.h>

static char sect_buffer[512];

static ata_device_t ata_device_list[4];

static void
identify_ata(unsigned short port, unsigned char dev, ata_device_t * target)
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
		goto err;
	}
	while (1) {
		if (inb(port + ATA_STATUS) & ATA_STATUS_ERR) {
			printk("ata bus[%i,%i]: no drive found\n",
			       port == PRIM_CHNN_BASE ? 0 : 1,
			       dev == ATA_MASTER_DRV ? 0 : 1);
			goto err;
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
	target->base_addr = port;
	target->device_select = dev;
	target->sector_count = *capacity;
	target->valid = ATA_VALID;
	return;
      err:
	target->valid = ATA_INVALID;
}

void
test()
{
	outb_p(PRIM_SECT, 1);
	outb_p(PRIM_LBALOW, 2);
	outb_p(PRIM_LBAMID, 0);
	outb_p(PRIM_LBAHI, 0);
	outb_p(PRIM_DRIVE, 0xe0);
	outb_p(PRIM_CMD, 0x20);
}

void
do_ata_int()
{
	printk("ATA INT!\n");
	unsigned short *buf = (unsigned short *) sect_buffer;
	for (int i = 0; i < 256; i++) {
		buf[i] = inw(PRIM_CHNN_BASE + ATA_DATA);
		printk("%x\n", buf[i]);
	}
}

void
ata_init()
{
	printk("scanning ATA bus\n");
	identify_ata(PRIM_CHNN_BASE, ATA_MASTER_DRV, &(ata_device_list[0]));
	identify_ata(PRIM_CHNN_BASE, ATA_SLAVE_DRV, &(ata_device_list[1]));
	identify_ata(SCND_CHNN_BASE, ATA_MASTER_DRV, &(ata_device_list[2]));
	identify_ata(SCND_CHNN_BASE, ATA_SLAVE_DRV, &(ata_device_list[3]));
	if (ata_device_list[0].valid == ATA_VALID
	    || ata_device_list[1].valid == ATA_VALID) {
		set_int_gate(PRIM_CHNN_VEC, &ata_prim_int);
		ioapic_enable(PRIM_CHNN_VEC, PRIM_CHNN_IRQ);
	}
	if (ata_device_list[2].valid == ATA_VALID
	    || ata_device_list[3].valid == ATA_VALID) {
		set_int_gate(SCND_CHNN_VEC, &ata_scnd_int);
		ioapic_enable(SCND_CHNN_VEC, SCND_CHNN_IRQ);
	}
}
