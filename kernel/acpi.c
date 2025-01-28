#include <acpi.h>
#include <lib/string.h>
#include <printk.h>
#include <stddef.h>
#include <panic.h>
#include <mm/mmap.h>

static rsdp_t *rsdp;

static rsdt_t *rsdt;
static madt_t *madt;

void *apic_addr;
void *ioapic_addr;

static unsigned char
acpi_checksum(char *head, unsigned int length)
{
	unsigned int sum = 0;;
	for (int i = 0; i < length; i++) {
		sum += head[i];
	}
	if ((sum & 0xff) == 0) {
		return 1;
	}
	return 0;
}

static xsdt_header_t *
find_xsdt(char *signature)
{
	xsdt_header_t **p = &(rsdt->next_xsdt);
	for (int i = 0;
	     i < (((rsdt->h.length) - sizeof (xsdt_header_t)) / sizeof (int *));
	     i++) {
		if (strncmp
		    ((char *) ((((*p) + 0xC0000000)->signature) + 0xC0000000),
		     signature, 4) == 0) {
			return (xsdt_header_t *) phy2lin(*p);
		}
		p++;
	}
	return NULL;
}

void
acpi_init()
{
	// search for RSDP
	char *p = (char *) phy2lin(RSDP_LO);
	while ((unsigned int) p < (unsigned int) phy2lin(RSDP_HI)) {
		if (memcmp(p, "RSD PTR ", 8) == 0) {
			break;
		}
		p += 16;
	}
	rsdp = (rsdp_t *) (p);
	// do checksum for all acpi versions
	if (acpi_checksum((char *) rsdp, sizeof (*rsdp)) == 0) {
		panic("bad rsdp checksum");
	}
	if (rsdp->revision != 0) {
		panic("unsupported acpi version");
	}
	rsdt = (rsdt_t *) (phy2lin(rsdp->rsdt_address));
	if (acpi_checksum((char *) rsdt, rsdt->h.length) == 0) {
		panic("bad rsdt checksum");
	}
	// find MADT
	madt = (madt_t *) find_xsdt("APIC");
	apic_addr = (void *) (madt->lapic_addr);
	unsigned int i;
	while (i < ((madt->h.length) - sizeof (xsdt_header_t))) {
		if ((madt->entries)[i] != 1) {
			i += (madt->entries)[i + 1];
		} else {
			ioapic_addr = *((void **) ((madt->entries) + i + 4));
			break;
		}
	}
	printk("%x\n", ioapic_addr);
}
