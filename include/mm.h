#define MEM_FAULT 0
#define MEM_UNUSED 1
#define MEM_USED 2

#define ZONE_KERNEL 1
#define ZONE_USER 2

#define NULL (void*)(0)

typedef struct{
	unsigned char status;
}__attribute__((packed)) page_t;

typedef struct SLAB slab_t;

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

#define SLAB_SIZE 4096*NR_SLAB_PAGE
#define NR_SLAB_PAGE 1

#define KMEM_FLAG_COMFORT 1
#define KMEM_FLAG_STRICT 2

void mm_init(unsigned int memupper, unsigned int memlower);

void slab_init();

void page_init(unsigned int);


void* palloc(int zone, unsigned int size); // kernel zone returns mapped address, \
										      user zone returns a physical address
void pfree(void* addr, unsigned int size, unsigned int zone);


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