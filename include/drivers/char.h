#ifndef DRIVERS_CHAR_H
#define DRIVERS_CHAR_H

#define MAJOR_NULL 0
#define MAJOR_MEM 1
#define MAJOR_TTY 2

#include <multiboot.h>

typedef struct {
	short major;
	int (*read)(short);
	int (*write)(short, char);
	int (*ioctl)(short, long);
} cdev_t;

void load_graphic();

void char_init();

int creat_cdev(short major, int (*read)(short), int(*write)(short, char),
	       int(*ioctl)(short, long));
int cread(short major, short minor);
int cwrite(short major, short minor, char data);
int cioctl(short major, short minor, long cmd);

#endif
