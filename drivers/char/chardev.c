#include "vga_text.h"
#include <stddef.h>

#define DEV_MAX 8

typedef struct{
	int(*read)(char*, int);
	int(*write)(char*, int);
	int(*readc)(char*);
	int(*writec)(char);
}cdev_t;

// char nodes list
static cdev_t cdev_list[] = {
	{&vga_text_read , &vga_text_write, &vga_text_readc, &vga_text_writec},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL}
};

int creadc(int dev, char* buf){
	if(dev>=DEV_MAX){
		return -1;
	}
	return cdev_list[dev].readc(buf);
}

int cwritec(int dev, char buf){
	if(dev>=DEV_MAX){
		return -1;
	}
	return cdev_list[dev].writec(buf);
}

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