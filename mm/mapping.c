#include <mm/buddy.h>
#include <cpu.h>
#include <asm/ring0.h>
#include <printk.h>
#include <lib/string.h>

int
map_page(pde_t * pdbr, void *phy, void *virt, unsigned char perm,
	 unsigned char rw)
{
	unsigned int pde_index = (unsigned int) virt >> 22;
	unsigned int pte_index = ((unsigned int) virt >> 12) & 0xfff;
	pde_t *pde = &pdbr[pde_index];
	pte_t *pte_base;
	pte_t *pte;
	if (pde->pse == 1) {
		// already mapped 4MB page
		return -1;
	}
	if (pde->present == 0) {
		// create page
		pte_base = palloc(ZONE_KERNEL, 1);	// this is page aligned
		memset(pte_base, 0, 4096);
		pde->present = 1;
		pde->rw = 1;	// page dir allow all access
		pde->us = 1;	// limit perm in page table
		pde->pwt = 0;
		pde->pcd = 0;
		pde->accessed = 0;
		pde->dirty = 0;
		pde->pse = 0;
		pde->avl = 0;
		pde->addr = (((unsigned int) pte_base - KERNEL_BASE) >> 12);
	} else {
		pte_base = (pte_t *) ((pde->addr) << 12);
	}
	pte = &pte_base[pte_index];
	if (pte->present == 1) {
		// already mapped page
		// unmmap first
		return -1;
	}
	pte->present = 1;
	pte->rw = rw;
	pte->us = perm;
	pte->pwt = 0;
	pte->pcd = 0;
	pte->accessed = 0;
	pte->dirty = 0;
	pte->reserved = 0;
	pte->global = 0;
	pte->avl = 0;
	pte->addr = ((unsigned int) phy) >> 12;
	flush_cr3();
}

int
map_huge_page(huge_pde_t * pdbr, void *phy, void *virt, unsigned char perm,
	      unsigned char rw)
{
	unsigned int pde_index = (unsigned int) virt >> 22;
	huge_pde_t *pde = &pdbr[pde_index];
	if (pde->present == 1) {
		// already mapped page(s)
		return -1;
	}
	pde->present = 1;
	pde->rw = rw;
	pde->us = perm;
	pde->pwt = 0;
	pde->pcd = 0;
	pde->accessed = 0;
	pde->dirty = 0;
	pde->pse = 1;
	pde->avl = 0;
	pde->reserved = 0;
	pde->addr = (unsigned int) phy >> 22;
}

void
unmap_page(pde_t * pdbr, void *virt)
{
	unsigned int pde_index = (unsigned int) virt >> 22;
	unsigned int pte_index = ((unsigned int) virt >> 12) & 0xfff;
	pde_t *pde = &pdbr[pde_index];
	pte_t *pte_base;
	pte_t *pte;
	if (pde->pse == 1) {
		// unmap 4MB page
		pde->present = 0;
		return;
	}
	pte_base = (pte_t *) (((pde->addr) << 12) + KERNEL_BASE);
	pte = &pte_base[pte_index];
	pte->present = 0;
	for (int i = 0; i < 1024; i++) {
		if (pte_base[i].present == 1) {
			goto no;
		}
	}
	pfree(pte_base, 1, ZONE_KERNEL);
	pde->present = 0;
      no:
	return;
}
