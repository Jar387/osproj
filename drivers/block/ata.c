#include <drivers/block/ata.h>
#include <drivers/pci.h>
#include <asm/ring0.h>
#include <printk.h>
#include <cpu.h>
#include <apic.h>
#include <mm/buddy.h>

static char *sect_buffer;

static ata_device_t ata_device_list[4];
static unsigned short bus_master_port[2];
char *prdt;

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
}

void
do_ata_int()
{
	outb_p(bus_master_port[0], 8);
	printk("ATA INT!\n");
	printk("%x %x\n", inb(bus_master_port[0] + 2), inb(PRIM_STATUS));
	unsigned short *buf = (unsigned short *) sect_buffer;
	printk("%x\n", buf[255]);
}

void
ata_init()
{
	sect_buffer = (char *) palloc(ZONE_KERNEL, PAGE_SIZE);
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
	pci_device_t *devices[2];
	int num =
	    get_device_by_class(CLASSCODE_MASS_STORAGE, SUBCLASS_IDE_CONTROLLER,
				devices, 2);
	if (num == 0) {
		printk("no DMA support\n");
	}
	if (num >= 1) {
		bus_master_port[0] = (devices[0]->bar[4]) & 0xFFFC;
	}
	if (num == 2) {
		bus_master_port[1] = (devices[1]->bar[4]) & 0xFFFC;
	}
	// PRDT test
	prdt = palloc(ZONE_KERNEL, PAGE_SIZE);
	int *prdt_i = (int *) prdt;
	prdt_i[0] = ((int) sect_buffer) - 0xC0000000;
	prdt_i[1] = 0x80000200;
	prdt -= 0xc0000000;
	outl_p(bus_master_port[0] + 4, prdt);
	outb_p(bus_master_port[0], 8);
	outb_p(bus_master_port[0] + 2, 0);
	outb_p(PRIM_SECT, 1);
	outb_p(PRIM_LBALOW, 0);
	outb_p(PRIM_LBAMID, 0);
	outb_p(PRIM_LBAHI, 0);
	outb_p(PRIM_DRIVE, 0xe0);
	outb_p(PRIM_CMD, 0xc8);
	outb_p(bus_master_port[0], 9);
}
