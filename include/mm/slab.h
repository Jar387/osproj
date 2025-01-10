#ifndef SLAB_H
#define SLAB_H

#include <stddef.h>
#include <mm/buddy.h>

#define KMEM_FLAG_COMFORT 1
#define KMEM_FLAG_STRICT 2

#define SLAB_SIZE 4096*NR_SLAB_PAGE
#define NR_SLAB_PAGE 1

void slab_init();

typedef struct SLAB slab_t;
typedef struct KMEM_CACHE kmem_cache_t;

void* __kmalloc(unsigned int size, unsigned int flags, kmem_cache_t* quickalloc);
// return a kernel-space mapped address(always use in kernel)
static inline void* kmalloc(unsigned int size){
	return __kmalloc(size, KMEM_FLAG_COMFORT, NULL);
}

static inline void* kmalloc_s(unsigned int size){
	return __kmalloc(size, KMEM_FLAG_STRICT, NULL);
}

static inline void* qsmalloc(kmem_cache_t* quickalloc){
	return __kmalloc(0, KMEM_FLAG_STRICT, quickalloc);
}

void kfree(void* addr);

kmem_cache_t* query_kmem(unsigned int size);


typedef struct KMEM_CACHE{
	slab_t* empty_slab;
	slab_t* partial_slab;
	slab_t* full_slab;
	unsigned int obj_size;
	unsigned int obj_count_per_slab;
	unsigned int flags;
	struct KMEM_CACHE* next;
}kmem_cache_t;

typedef struct FREE{
	unsigned int index;
	struct FREE* next;
}free_t;

typedef struct SLAB{
	void* start;
	unsigned int free_count;
	kmem_cache_t* parent;
	free_t* free;
	struct SLAB* next;
}slab_t;

#endif