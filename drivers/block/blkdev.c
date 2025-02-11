#include <drivers/block.h>
#include <drivers/block/ata.h>
#include <mm/slab.h>
#include <stddef.h>
#include <lib/bst.h>
#include <errno.h>

static bst_node_t *blkdev_root;

void
block_init()
{
	ata_init();
}

int
creat_bdev(short major, int (*read)(short, char *, int),
	   int(*write)(short, char *, int), int(*ioctl)(short, long))
{
	bdev_t *newdev =(bdev_t *) kmalloc(sizeof (*newdev));
	newdev->major = major;
	newdev->read = read;
	newdev->write = write;
	newdev->ioctl = ioctl;
	bst_insert(&blkdev_root, newdev, (int) major);
}

int
bread(short major, short minor, char *buf, int count)
{
	bdev_t *dev = bst_search(blkdev_root, (int) major);
	if (dev == NULL) {
		return -ENODEV;
	}
	if (dev->read == NULL) {
		return -ENODEV;
	}
	return (dev->read) (minor, buf, count);
}

int
bwrite(short major, short minor, char *buf, int count)
{
	bdev_t *dev = bst_search(blkdev_root, (int) major);
	if (dev == NULL) {
		return -ENODEV;
	}
	if (dev->write == NULL) {
		return -ENODEV;
	}
	return (dev->write) (minor, buf, count);
}

int
bioctl(short major, short minor, long cmd)
{
	bdev_t *dev = bst_search(blkdev_root, (int) major);
	if (dev == NULL) {
		return -ENODEV;
	}
	if (dev->ioctl == NULL) {
		return -ENODEV;
	}
	return (dev->ioctl) (minor, cmd);
}
