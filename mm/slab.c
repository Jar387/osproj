#include <mm.h>

static kmem_cache_t kmem_kmem{
	.empty_slab = NULL;
	.partial_slab = NULL;
	.full_slab = NULL;
	.obj_size = sizeof(kmem_cache_t);
	.obj_count_per_slab = SLAB_SIZE/sizeof(kmem_cache_t);
	.flags = KMEM_FLAG_STRICT;
	.next = NULL;
};

static slab_t* creat_slab(unsigned int obj_count_per_slab){
	slab_t* slab = palloc(ZONE_KERNEL, NR_SLAB_PAGE);
	slab->free_count = obj_count_per_slab;
	slab->next = NULL;
	free_t* freeidx = (free_t*)(slab++);
	free_t* prev = freeidx;
	for(int i=0;i<obj_count_per_slab;i++){
		freeidx->index = i;
		freeidx++;
		prev
	}
}

void slab_init(){

}