#include <lib/fifo.h>
#include <mm/slab.h>
#include <mm/buddy.h>
#include <lib/string.h>
#include <asm/ring0.h>

fifo_t *
alloc_fifo(unsigned int size, unsigned int data_size)
{
	if (size % data_size != 0) {
		size += (data_size - (size % data_size));
	}
	fifo_t *fifo = (fifo_t *) kmalloc(sizeof (*fifo));
	fifo->data_size = data_size;
	fifo->size = size;
	fifo->dirty = 0;
	fifo->data = palloc(ZONE_KERNEL, size);
	fifo->start = fifo->data;
	fifo->end = fifo->data;
	return fifo;
}

int
fifo_enqueue(fifo_t * fifo, void *data)
{
	fifo->dirty = 1;
	memcpy(fifo->end, data, fifo->data_size);
	// step end
	if ((fifo->end + fifo->data_size) >= (fifo->data + fifo->size)) {
		fifo->end = fifo->data;
		return -1;
	} else {
		fifo->end += fifo->data_size;
	}
	return 0;
}

int
fifo_dequeue(fifo_t * fifo, void *buffer)
{
	if (fifo->start == fifo->end && fifo->dirty == 1) {
		memset(buffer, 0, fifo->data_size);
		return -1;
	}
	memcpy(buffer, fifo->start, fifo->data_size);
	if ((fifo->start + fifo->data_size) >= (fifo->data + fifo->size)) {
		fifo->start = fifo->data;
	} else {
		fifo->start += fifo->data_size;
	}
	return 0;
}
