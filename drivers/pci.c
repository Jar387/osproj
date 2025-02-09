#include <asm/ring0.h>
#include <drivers/pci.h>
#include <lib/list.h>
#include <mm/slab.h>
#include <printk.h>
#include <stddef.h>

static list_node_t *pci_device_list;

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
	unsigned int device_id = read_pci_data(bus, dev, func, 0x0);
	unsigned int dev_id = read_pci_data(bus, dev, func, 0x8);
	unsigned char classcode = (unsigned char) ((dev_id >> 24) & 0xFF);
	unsigned char subclass = (unsigned char) ((dev_id >> 16) & 0xFF);
	unsigned char progif = (unsigned char) ((dev_id >> 8) & 0xFF);
	pci_device_t *device = (pci_device_t *) kmalloc(sizeof (*device));
	list_insert(&pci_device_list, device);
	device->bus = bus;
	device->classcode = classcode;
	device->device = dev;
	device->device_id = (device_id >> 16) & 0xFFFF;
	device->vendor_id = device_id & 0xFFFF;
	device->function = func;
	device->prog_if = progif;
	device->subclass = subclass;
	for (int i = 0x10, j = 0; i <= 0x24; i += 4, j++) {
		device->bar[j] = read_pci_data(bus, dev, func, i);
	}
	printk("%x2:%x2.%x1 %x4:%x4 [%x2%x2] ", bus, dev, func,
	       device->device_id, device->vendor_id, classcode, subclass);
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
				printk(" [PI %x2]", progif);
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
pci_init()
{
	// detect pci
	if (!check_device(0, 0, 0)) {
		printk("no pci exist\n");
		return;
	}
	printk("scanning pci\n");
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

static pci_device_t *swap1;
static unsigned char bus_swap;
static unsigned char device_swap;
static unsigned char function_swap;
static int
callback1(list_node_t * node)
{
	pci_device_t *dev = (pci_device_t *) (node->data);
	if (dev->bus == bus_swap && dev->device == device_swap
	    && dev->function == function_swap) {
		swap1 = dev;
		return 1;
	}
	return 0;
}

pci_device_t *
get_device_by_id(unsigned char bus, unsigned char device,
		 unsigned char function)
{
	swap1 = NULL;
	bus_swap = bus;
	device_swap = device;
	function_swap = function;
	list_iter(pci_device_list, &callback1);
	return swap1;
}

static unsigned char classcode_swap;
static unsigned char subclass_swap;
static int bufsz_swap;
static pci_device_t **buf_swap;
static int index_swap;

static int
callback2(list_node_t * node)
{
	pci_device_t *dev = (pci_device_t *) (node->data);
	if (dev->classcode == classcode_swap && dev->subclass == subclass_swap
	    && (bufsz_swap == 0 || bufsz_swap > index_swap)) {
		buf_swap[index_swap] = dev;
		index_swap++;
		return 1;
	}
	return 0;
}

int
get_device_by_class(unsigned char classcode, unsigned char subclass,
		    pci_device_t ** buf, int bufsz)
{
	index_swap = 0;
	classcode_swap = classcode;
	subclass_swap = subclass;
	buf_swap = buf;
	bufsz_swap = bufsz;
	list_iter(pci_device_list, &callback2);
	return index_swap;
}
