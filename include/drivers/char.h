#ifndef DRIVERS_CHAR_H
#define DRIVERS_CHAR_H

#define CDEV_STDOUT 1
#define CDEV_STDIN 2
#define CDEV_STDERR 3

#include <multiboot.h>

typedef struct CDEV {
	int dev_num;
	int (*read)(char *, int);
	int (*write)(char *, int);
	int (*ioctl)(long);
	struct CDEV *next;
} cdev_t;

int cdev_preload(struct multiboot_info *info);

int creat_cdev(int dev_num, int (*read)(char *, int), int(*write)(char *, int),
	       int(*ioctl)(long));
int cread(int dev, char *buf, int count);
int cwrite(int dev, char *buf, int count);
int ioctl(int dev, long cmd);

#endif
