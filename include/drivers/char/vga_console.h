#ifndef DRIVERS_CHAR_VGA_CONSOLE_H
#define DRIVERS_CHAR_VGA_CONSOLE_H

#include <drivers/char.h>

#define VGA_CONSOLE_IOCMD_CLS 1
void vga_console_init(struct multiboot_info *info, cdev_t * stdout);

#endif
