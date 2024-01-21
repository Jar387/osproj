#define MEM_FAULT 0
#define MEM_UNUSED 1
#define MEM_USED 2

#define ZONE_KERNEL 1
#define ZONE_USER 2

typedef struct{
	unsigned char status;
}__attribute__((packed)) page_t;

void mm_init(unsigned int memupper, unsigned int memlower);
void* palloc(int zone, unsigned int size);
void pfree(void* virtual, unsigned int size);