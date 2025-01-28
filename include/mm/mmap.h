#ifndef MAPPING_H
#define MAPPING_H

#include <cpu.h>

#define PAGE_READ 0
#define PAGE_RW 1

#define PAGE_SUPER 0
#define PAGE_USER 1

// kernel space phy:0x0~0x3FFFFFFF map to liner:0xC0000000~0xFFFFFFFF
// mmio device phy:0xC0000000~0xFFFFFFFF map to liner:0x80000000~0xBFFFFFFF

int map_page(pde_t * pdbr, void *phy, void *virt, unsigned char perm,
	     unsigned char rw);
int map_huge_page(pde_t * pdbr, void *phy, void *virt, unsigned char perm,
		  unsigned char rw);
void unmap_page(pde_t * pdbr, void *virt);

void *phy2lin(void *phy);
void *lin2phy(void *lin);

#endif
