#define MEM_FAULT 0
#define MEM_UNUSED 1
#define MEM_USED 2

#define ZONE_KERNEL 1
#define ZONE_USER 2

typedef struct{
	unsigned char status;
}__attribute__((packed)) page_t;

typedef struct FREE{
	unsigned int index;
	struct FREE* next;
}free_t;

typedef struct SLAB{
	void* start;
	unsigned int free_count;
	free_t* free;
	struct SLAB* next;
}slab_t;

#define SLAB_SIZE 4096*NR_SLAB_PAGE
#define NR_SLAB_PAGE 1

typedef struct KMEM_CACHE{
	slab_t* empty_slab;
	slab_t* partial_slab;
	slab_t* full_slab;
	unsigned int obj_size;
	unsigned int obj_count_per_slab;
	unsigned int flags;

	struct KMEM_CACHE* next;
}kmem_cache_t;

#define KMEM_FLAG_COMFORT 1
#define KMEM_FLAG_STRICT 2

void mm_init(unsigned int memupper, unsigned int memlower);
void* palloc(int zone, unsigned int size);
void pfree(void* virtual, unsigned int size);