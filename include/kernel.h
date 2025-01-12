#ifndef KERNEL_H
#define KERNEL_H

#include <multiboot.h>

void kmain(struct multiboot_info *info);
void init();

#endif
