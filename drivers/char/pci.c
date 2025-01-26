#include <asm/ring0.h>
#include <drivers/char/pci.h>
#include <printk.h>
#include <stddef.h>

static char *classes[256] =
    { "Unclassified", "Mass Storage Controller", "Network Controller",
	"Display Controller", "Multimedia Controller", "Memory Controller",
	"Bridge",
	"Simple Communication Controller", "Base System Peripheral",
	"Input Device Controller",
	"Docking Station", "Processor", "Serial Bus Controller",
	"Wireless Controller",
	"Intelligent Controller", "Satellite Communication Controller",
	"Encryption Controller",
	"Signal Processing Controller", "Processing Accelerator",
	"Non-Essential Instrumentation"
};

static char *mass_storage[] =
    { "SCSI Bus Controller", "IDE Controller", "Floppy Disk Controller",
	"IPI Bus Controller", "RAID Controller", "ATA Controller",
	"Serial ATA Controller",
	"Serial Attached SCSI Controller", "Non-Volatile Memory Controller"
};

static unsigned int
read_pci_data(unsigned char bus, unsigned char dev, unsigned char func,
	      unsigned char offset)
{
	unsigned int address = (unsigned int) ((bus << 16) | (dev << 11) |
					       (func << 8) | (offset & 0xFC) |
					       ((unsigned int) 0x80000000));
	outl(PCI_CONFIG_ADDRESS, address);
	return inl(PCI_CONFIG_DATA);
}

static unsigned short
read_pci_word(unsigned char bus, unsigned char dev, unsigned char func,
	      unsigned char offset)
{
	return (unsigned
		short) ((read_pci_data(bus, dev, func, offset) >>
			 ((offset & 2) * 8)) & 0xFFFF);
}

static unsigned char
read_pci_byte(unsigned char bus, unsigned char dev, unsigned char func,
	      unsigned char offset)
{
	return (unsigned
		char) ((read_pci_word(bus, dev, func, offset) >>
			((offset & 1) * 8)) & 0xFF);
}

static unsigned char
check_device(unsigned char bus, unsigned char dev, unsigned char func)
{
	if (read_pci_word(bus, dev, func, 0) == 0xFFFF) {
		return 0;
	}
	return 1;
}

static void
classfy_device(unsigned char bus, unsigned char dev, unsigned char func)
{
	unsigned int dev_id = read_pci_data(bus, dev, func, 0x8);
	unsigned char classcode = (unsigned char) ((dev_id >> 24) & 0xFF);
	unsigned char subclass = (unsigned char) ((dev_id >> 16) & 0xFF);
	unsigned char progif = (unsigned char) ((dev_id >> 8) & 0xFF);
	printk("[pci:%i:%i:%i] ", bus, dev, func);
	char *classname = classes[classcode];
	if (classname != NULL) {
		// add new device support here
		if (classcode == 0x1) {
			classname = mass_storage[subclass];
			if (classname != NULL) {
				printk("%s", classname);
			}
			if (subclass == 0x1) {
				// ide controller

			}
		} else {
			printk("%s", classname);
		}
		printk("\n");
	} else {
		printk("unknown device\n");
		return;
	}
}

void
init_pci()
{
	// detect pci
	if (!check_device(0, 0, 0)) {
		printk("no pci exist\n");
		return;
	}
	// scan device
	for (int bus = 0; bus < 256; bus++) {
		for (int dev = 0; dev < 32; dev++) {
			for (int func = 0; func < 8; func++) {
				if (check_device(bus, dev, func) == 0) {
					continue;
				}
				classfy_device(bus, dev, func);
			}
		}
	}
}
