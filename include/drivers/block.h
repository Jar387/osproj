#ifndef DRIVERS_BLOCK_H
#define DRIVERS_BLOCK_H

typedef struct bdev_t {
	short major;
	int (*read)(short, char *, int);
	int (*write)(short, char *, int);
	int (*ioctl)(short, long);
} bdev_t;

void block_init();
int creat_bdev(short major, int (*read)(short, char *, int),
	       int (*write)(short, char *, int), int (*ioctl)(short, long));
int bread(short major, short minor, char *buf, int count);
int bwrite(short major, short minor, char *buf, int count);
int bioctl(short major, short minor, long cmd);

#endif
