#include "vga_text.h"
#include <stddef.h>

#define DEV_MAX 8

typedef struct{
	int(*read)(char*, int);
	int(*write)(char*, int);
}cdev_t;

// char nodes list
static cdev_t cdev_list[] = {
	{&vga_text_read , &vga_text_write},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL}
};

int cread(int dev, char* buf, int count){
	if(dev>=DEV_MAX){
		return -1;
	}
	return cdev_list[dev].read(buf, count);
}

int cwrite(int dev, char* buf, int count){
	if(dev>=DEV_MAX){
		return -1;
	}
	return cdev_list[dev].write(buf, count);
}