#ifndef FIFO_H
#define FIFO_H

typedef struct {
	unsigned int size;
	unsigned int data_size;
	void *data;
	void *start;
	void *end;
} fifo_t;

fifo_t *alloc_fifo(unsigned int size, unsigned int data_size);
void fifo_enqueue(fifo_t * fifo, void *data);
void fifo_dequeue(fifo_t * fifo, void *buffer);

#endif
