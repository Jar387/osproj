#ifndef ACPI_H
#define ACPI_H

#define RSDP_LO (void*)0xe0000
#define RSDP_HI (void*)0xfffff

typedef struct {
	char signature[8];
	unsigned char checksum;
	char OEMID[6];
	unsigned char revision;
	void *rsdt_address;
} __attribute__ ((packed)) rsdp_t;

typedef struct {
	char signature[4];
	unsigned int length;
	unsigned char revision;
	unsigned char checksum;
	char OEMID[6];
	char OEMTableID[8];
	unsigned int OEM_revision;
	unsigned int creator_ID;
	unsigned int creator_revision;
} __attribute__ ((packed)) xsdt_header_t;

typedef struct {
	xsdt_header_t h;
	xsdt_header_t *next_xsdt;
} __attribute__ ((packed)) rsdt_t;

typedef struct {
	xsdt_header_t h;
	unsigned int lapic_addr;
	unsigned int flags;
	unsigned char entries[];
} __attribute__ ((packed)) madt_t;

void acpi_init();

extern void *apic_addr;
extern void *ioapic_addr;

#endif
