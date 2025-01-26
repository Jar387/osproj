#include <stddef.h>
#include <mm/slab.h>
#include <multiboot.h>
#include <drivers/char/vga_console.h>
#include <drivers/char/pci.h>
#include <drivers/char.h>

static cdev_t preload_console = {
	.dev_num = CDEV_STDOUT,
	.read = NULL,
	.write = NULL,
	.ioctl = NULL,
	.next = NULL
};

// char nodes list
static cdev_t *head;
static cdev_t *tail;

// quick-search cache
static cdev_t *stdout;
static cdev_t *stdin;
static cdev_t *stderr;
static cdev_t *last;

int
cdev_preload(struct multiboot_info *info)
{
	stdout = &preload_console;
	stderr = &preload_console;
	vga_console_init(info, stdout);
}

int
cdev_load(struct multiboot_info *info)
{
	init_pci();
}

int
creat_cdev(int dev_num, int (*read)(char *, int), int(*write)(char *, int),
	   int(*ioctl)(long))
{
	cdev_t *new_dev = kmalloc(sizeof(cdev_t));
	if (head == NULL) {
		head = new_dev;
		tail = head;
	} else {
		tail->next = new_dev;
		tail = new_dev;
	}
	new_dev->dev_num = dev_num;
	new_dev->read = read;
	new_dev->write = write;
	new_dev->ioctl = ioctl;
	new_dev->next = NULL;
}

static cdev_t *
search_cdev(int dev)
{
	cdev_t *do_dev = head;
	// try to hit cache
	if (stdin != NULL) {
		if (dev == stdin->dev_num) {
			return stdin;
		}
	}
	if (stdout != NULL) {
		if (dev == stdout->dev_num) {
			return stdout;
		}
	}
	if (stderr != NULL) {
		if (dev == stderr->dev_num) {
			return stderr;
		}
	}
	if (last != NULL) {
		if (dev == last->dev_num) {
			return last;
		}
	}
	// cache not hit
	if (head == NULL) {
		return NULL;
	}
	while (do_dev->next != NULL) {
		if (do_dev->dev_num == dev) {
			last = do_dev;
			return do_dev;
		}
		do_dev = do_dev->next;
	}

	return NULL;
}

int
cread(int dev, char *buf, int count)
{
	cdev_t *cdev = search_cdev(dev);
	if (cdev != NULL && cdev->read != NULL) {
		return cdev->read(buf, count);
	}
	return -1;
}

int
cwrite(int dev, char *buf, int count)
{
	cdev_t *cdev = search_cdev(dev);
	if (cdev != NULL && cdev->write != NULL) {
		return cdev->write(buf, count);
	}
	return -1;
}

int
ioctl(int dev, long cmd)
{
	cdev_t *cdev = search_cdev(dev);
	if (cdev != NULL && cdev->ioctl != NULL) {
		return cdev->ioctl(cmd);
	}
	return -1;
}
