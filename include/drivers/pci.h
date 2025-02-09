#ifndef PCI_H
#define PCI_H

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

void pci_init();

typedef struct {
	unsigned char bus;
	unsigned char device;
	unsigned char function;
	unsigned short device_id;
	unsigned short vendor_id;
	unsigned char classcode;
	unsigned char subclass;
	unsigned char prog_if;
	unsigned int bar[6];
} pci_device_t;

int get_device_by_class(unsigned char classcode, unsigned char subclass,
			pci_device_t ** buf, int bufsz);
pci_device_t *get_device_by_id(unsigned char bus, unsigned char device,
			       unsigned char function);

#endif
