#ifndef MAPPING_H
#define MAPPING_H

#define PAGE_READ 0
#define PAGE_RW 1

#define PAGE_SUPER 0
#define PAGE_USER 1

int map_page(pde_t* pdbr, void* phy, void* virt, unsigned char perm, unsigned char rw);
int map_huge_page(pde_t* pdbr, void* phy, void* virt, unsigned char perm, unsigned char rw);
void unmap_page(pde_t* pdbr, void* virt);

#endif