#include <drivers/block.h>
#include <drivers/block/ata.h>
#include <mm/slab.h>
#include <stddef.h>

static bdev_t *head;
static bdev_t *tail;

// quick table
static bdev_t *hda;
static bdev_t *root_part;
static bdev_t *last;

void
blkdev_load()
{
	// ata_init();
}

int
creat_bdev(int dev_num, int (*read)(char *, int), int(*write)(char *, int),
	   int(*ioctl)(long), int(*seek)(long, int))
{
	bdev_t *new_dev = kmalloc(sizeof(bdev_t));
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
	new_dev->seek = seek;
	new_dev->next = NULL;
}

static bdev_t *
search_bdev(int dev)
{
	bdev_t *do_dev = head;
	// try to hit cache
	if (hda != NULL) {
		if (dev == hda->dev_num) {
			return hda;
		}
	}
	if (root_part != NULL) {
		if (dev == root_part->dev_num) {
			return root_part;
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
bread(int dev, char *buf, int count)
{
	bdev_t *bdev = search_bdev(dev);
	if (bdev != NULL && bdev->read != NULL) {
		return bdev->read(buf, count);
	}
	return -1;
}

int
bwrite(int dev, char *buf, int count)
{
	bdev_t *bdev = search_bdev(dev);
	if (bdev != NULL && bdev->write != NULL) {
		return bdev->write(buf, count);
	}
	return -1;
}

int
bioctl(int dev, long cmd)
{
	bdev_t *bdev = search_bdev(dev);
	if (bdev != NULL && bdev->ioctl != NULL) {
		return bdev->ioctl(cmd);
	}
	return -1;
}

int
bseek(int dev, long offset, int method)
{
	bdev_t *bdev = search_bdev(dev);
	if (bdev != NULL && bdev->seek != NULL) {
		return bdev->seek(offset, method);
	}
	return -1;
}
