#include <stddef.h>
#include <mm/slab.h>
#include <errno.h>
#include <lib/bst.h>
#include <drivers/char/devmem.h>
#include <drivers/char/tty.h>
#include <drivers/char/pci.h>
#include <drivers/char.h>
bst_node_t *chardev_root;

void
load_graphic()
{
	preinit_tty();
}

void
cdev_load()
{
	init_devmem();
	init_tty();
	init_pci();
}

int
creat_cdev(short major, int (*read)(short), int (*write)(short, char),
	   int (*ioctl)(short, long))
{
	cdev_t *newdev = (cdev_t *) kmalloc(sizeof(*newdev));
	newdev->major = major;
	newdev->read = read;
	newdev->write = write;
	newdev->ioctl = ioctl;
	bst_insert(&chardev_root, newdev, (int) major);
}

int
cread(short major, short minor)
{
	cdev_t *dev = bst_search(chardev_root, (int) major);
	if (dev == NULL) {
		return -ENODEV;
	}
	if (dev->read == NULL) {
		return -ENODEV;
	}
	return (dev->read) (minor);
}

int
cwrite(short major, short minor, char data)
{
	cdev_t *dev = bst_search(chardev_root, (int) major);
	if (dev == NULL) {
		return -ENODEV;
	}
	if (dev->write == NULL) {
		return -ENODEV;
	}
	return (dev->write) (minor, data);
}

int
cioctl(short major, short minor, long cmd)
{
	cdev_t *dev = bst_search(chardev_root, (int) major);
	if (dev == NULL) {
		return -ENODEV;
	}
	if (dev->ioctl == NULL) {
		return -ENODEV;
	}
	return (dev->ioctl) (minor, cmd);
}
