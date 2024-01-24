#ifndef BUDDY_H
#define BUDDY_H

#include <stddef.h>

void page_init(unsigned int);

void* palloc(int zone, unsigned int size); // kernel zone returns mapped address, \
										      user zone returns a physical address
void pfree(void* addr, unsigned int size, unsigned int zone);

#define MEM_FAULT 0
#define MEM_UNUSED 1
#define MEM_USED 2

#define ZONE_KERNEL 1
#define ZONE_USER 2

typedef struct{
	unsigned char status;
}__attribute__((packed)) page_t;

extern unsigned int _kernel_start;
extern unsigned int _kernel_end;

// I don't know why must lea them to access
// But it works :)
#define KERNEL_START (unsigned int)(&_kernel_start)
#define KERNEL_END ((unsigned int)(&_kernel_end))-0xc0000000

#endif