#ifndef FIFO_H
#define FIFO_H

typedef struct {
	unsigned int size;
	unsigned int data_size;
	int dirty;
	char *data;
	char *start;
	char *end;
} fifo_t;

fifo_t *alloc_fifo(unsigned int size, unsigned int data_size);
int fifo_enqueue(fifo_t * fifo, void *data);
int fifo_dequeue(fifo_t * fifo, void *buffer);

#endif
