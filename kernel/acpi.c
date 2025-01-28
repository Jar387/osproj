#include <acpi.h>
#include <lib/string.h>
#include <printk.h>
#include <stddef.h>

static rsdp_t *rsdp;

static rsdt_t *rsdt;

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
			return ACPI_PTR(*p);
		}
		p++;
	}
	return NULL;
}

void
acpi_init()
{
	// search for RSDP
	char *p = (char *) ACPI_PTR(RSDP_LO);
	while ((unsigned int) p < ACPI_PTR(RSDP_HI)) {
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
		panic("unsupported apci version");
	}
	rsdt = (rsdt_t *) (ACPI_PTR(rsdp->rsdt_address));
	if (acpi_checksum((char *) rsdt, rsdt->h.length) == 0) {
		panic("bad rsdt checksum");
	}
	printk("%x\n", find_xsdt("APIC"));
}
