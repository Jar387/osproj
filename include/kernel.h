#ifndef KERNEL_H
#define KERNEL_H

#include <multiboot.h>

#define ASSERT(statement) ({ \
    if(!statement){ \
        panic("assert failed"); \
    } \
})

void kmain(struct multiboot_info *info);
void init();

#endif
