#include <lib/fifo.h>
#include <mm/slab.h>
#include <mm/buddy.h>

fifo_t *
alloc_fifo(unsigned int size, unsigned int data_size)
{
	fifo_t *fifo = (fifo_t *) kmalloc(sizeof (*fifo));
	fifo->data_size = data_size;
	fifo->size = size;
	fifo->data = palloc(ZONE_KERNEL, size);
}

void fifo_enqueue(fifo_t * fifo, void *data);
void fifo_dequeue(fifo_t * fifo, void *buffer);
