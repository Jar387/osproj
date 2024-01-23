#include <mm.h>
#include <stddef.h>
#include <printk.h>
#include <asm/cpuins.h>

// alloctor struct stub to allocate allocator
static kmem_cache_t kmem_kmem = {
	.empty_slab = NULL,
	.partial_slab = NULL,
	.full_slab = NULL,
	.obj_size = sizeof(kmem_cache_t),
	.obj_count_per_slab = (SLAB_SIZE-sizeof(slab_t))/(sizeof(free_t)+sizeof(kmem_cache_t)),
	.flags = KMEM_FLAG_STRICT,
	.next = NULL
};

static kmem_cache_t* kmem_t_tail = &kmem_kmem;

void* alloc_from_kmem(kmem_cache_t* kmem);

static inline kmem_cache_t* alloc_kmem_t(){
	return (kmem_cache_t*)alloc_from_kmem(&kmem_kmem);
}

kmem_cache_t* add_kmem(unsigned int obj_size, unsigned int flags){
	kmem_cache_t* kmem = alloc_kmem_t();
	kmem->empty_slab = NULL;
	kmem->partial_slab = NULL;
	kmem->full_slab = NULL;
	kmem->obj_size = obj_size;
	kmem->obj_count_per_slab = (SLAB_SIZE-sizeof(slab_t))/(sizeof(free_t)+obj_size);
	kmem->flags = flags;
	kmem->next = NULL;
	kmem_t_tail->next = kmem;
	kmem_t_tail = kmem;
	return kmem;
}

slab_t* release_slab(void* obj){
	slab_t* slab = (slab_t*)((unsigned int)obj&0xfffff000); // slab is page aligned
	unsigned int index = ((unsigned int)obj-(unsigned int)(slab->start))/(slab->parent->obj_size);
	free_t* free_arr = slab->free;
	free_t* free_node = &free_arr[index];
	free_node->next = slab->free;
	slab->free = free_node;
	slab->free_count++;
	return slab;
}

void* __kmalloc(unsigned int size, unsigned int flags){
	
}

void kfree(void* addr){
	if(addr<0xc0000000){
		return;
	}
	slab_t* slab = release_slab(addr);
	slab_t* head;
	if(slab->free_count==1){
		// must in full
		head = slab->parent->full_slab;
	}else{
		// must in partial
		head = slab->parent->partial_slab;
	}
	if(head==slab){
		goto searchfin;
	}
	while(head->next!=slab){
		head = head->next;
	}
	head->next = slab->next;
	if(slab->free_count==slab->parent->obj_count_per_slab){
		// mov to empty
		slab->next = slab->parent->empty_slab;
		slab->parent->empty_slab = slab;
	}else{
		// mov to partial
		slab->next = slab->parent->partial_slab;
		slab->parent->partial_slab = slab;
	}
}


slab_t* creat_slab(unsigned int obj_count){
	slab_t* slab = palloc(ZONE_KERNEL, NR_SLAB_PAGE);
	slab->free_count = obj_count;
	slab->next = NULL;
	free_t* freeidx = (free_t*)(slab+1);
	slab->free = freeidx;
	free_t* prev = freeidx;
	for(int i=0;i<obj_count;i++){
		freeidx->index = i;
		freeidx++;
		prev->next = freeidx;
		prev++;
	}
	prev--;
	prev->next = NULL; // cut tail
	slab->start = freeidx;
	return slab;
}

void* alloc_from_slab(slab_t* slab, unsigned int obj_size){
	free_t* alloc = slab->free;
	slab->free = alloc->next;
	slab->free_count--;
	unsigned int index = alloc->index;
	return (void*)((unsigned int)(slab->start)+obj_size*index);
}

void* alloc_from_kmem(kmem_cache_t* kmem){
	slab_t* head = kmem->partial_slab;
	void* ret;
	int status = 0; // from partial
	if(kmem->partial_slab==NULL){
		if(kmem->empty_slab==NULL){
			// need alloc more slab!
			head = creat_slab(kmem->obj_count_per_slab);
			head->parent = kmem;
			kmem->empty_slab = head;
			goto creat_fin;
		}
		head = kmem->empty_slab;
		status = 1; // from empty
	}
	creat_fin:
	ret = alloc_from_slab(head, kmem->obj_size);
	if(status==0){
		if(head->free_count==0){
			// mov to full
			kmem->partial_slab = head->next;
			head->next = kmem->full_slab;
			kmem->full_slab = head;
		}else{
			// do nothing
		}
	}else{
		if(head->free_count==0){
			// ultra-large object: one object in one slab
			kmem->empty_slab = head->next;
			head->next = kmem->full_slab;
			kmem->full_slab = head;
		}else{
			// mov to partial
			kmem->empty_slab = head->next;
			head->next = kmem->partial_slab;
			kmem->partial_slab = head;
		}
	}
	return ret;
}

void slab_init(){
	// init kmemcache alloctor
}