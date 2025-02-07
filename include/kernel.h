#ifndef KERNEL_H
#define KERNEL_H

#include <multiboot.h>

#define ASSERT(statement) ({ \
    if(!statement){ \
        panic("assert failed"); \
    } \
})

extern struct multiboot_info *multiboot_config;
void kmain(struct multiboot_info *info);
void init();

#endif
