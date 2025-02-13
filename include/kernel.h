#ifndef KERNEL_H
#define KERNEL_H

#include <multiboot.h>

#define ASSERT(statement) ({ \
    if(!statement){ \
        panic("assert failed"); \
    } \
})

typedef struct {
	unsigned short serial_addr[4];
	unsigned short parallel_addr[3];
	unsigned short fake_ebda;
	unsigned char trash[136];
} __attribute__ ((packed)) bios_data_t;

extern bios_data_t _biosdata;
#define BIOS_DATA ((bios_data_t*)(void*)(&_biosdata))

extern struct multiboot_info *multiboot_config;
void kmain(struct multiboot_info *info);
void init();

#endif
