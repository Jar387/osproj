#ifndef ACPI_H
#define ACPI_H

#define ACPI_PTR(p) (((char*)p)+0xC0000000)

#define RSDP_LO 0xe0000
#define RSDP_HI 0xfffff

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

void acpi_init();

#endif
