#ifndef DRIVERS_BLOCK_H
#define DRIVERS_BLOCK_H

typedef struct BDEV{
	int dev_num;
	int(*read)(char*, int);
	int(*write)(char*, int);
	int(*ioctl)(long);
	int(*seek)(long, int);
	struct BDEV* next;
}bdev_t;

void blkdev_load();
int creat_bdev(int dev_num, int(*read)(char*, int), int(*write)(char*, int), int(*ioctl)(long), int(*seek)(long, int));
int bread(int dev, char* buf, int count);
int bwrite(int dev, char* buf, int count);
int bioctl(int dev, long cmd);
int bseek(int dev, long offset, int method);

#endif