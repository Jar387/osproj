#include <cpu.h>
#include <mm.h>
#include <printk.h>
#include <stddef.h>

// I don't know why must lea them to access
// But it works :)
extern unsigned int _kernel_start;
extern unsigned int _kernel_end;
#define KERNEL_START (unsigned int)(&_kernel_start)
#define KERNEL_END ((unsigned int)(&_kernel_end))-0xc0000000

static page_t* memmap_kernel;
static page_t* memmap_user;
static page_t* memmap_end;

static unsigned int kernel_mem_pages;
static unsigned int user_mem_pages;

void mm_init(unsigned int memupper, unsigned int memlower){
	unsigned int total_mem = ((memupper+1024)&0xfffff800)+2048; // um, it works :)
	printk("mem total %i kb\n", total_mem);
	kernel_mem_pages = total_mem>>4;
	user_mem_pages = (total_mem>>4)*3;
	// build physical mem track bytemap
	// kernel uses low physical mem 0x0-0x3fffffff that mapped to 0xc0000000-0xffffffff
	// user uses high physical mem 0x40000000-0xffffffff that mapped to 0x0-0xbfffffff
	// address space is dynamicly changed within actually physical mem size
	memmap_kernel = (page_t*)KERNEL_END;
	memmap_user = (page_t*)(KERNEL_END+kernel_mem_pages);
	memmap_end = memmap_user+user_mem_pages;
	for(page_t* ptr=memmap_kernel;ptr<memmap_end;ptr++){
		ptr->status = MEM_UNUSED;
	}
}

void* alloc_a_page(int zone){ // return a physical address
	page_t* ptr;
	unsigned int searchlen;
	unsigned int index=0;
	if(zone==ZONE_KERNEL){
		ptr=memmap_kernel;
		searchlen=kernel_mem_pages;
	}if(zone==ZONE_USER){
		ptr=memmap_user;
		searchlen=user_mem_pages;
	}else{
		return NULL;
	}
	for(;index<searchlen;index++){
		if(ptr[index]->status=MEM_UNUSED){
			ptr[index]->status=MEM_USED;
			if(zone=ZONE_KERNEL){
				return (void*)(index*4096);
			}else{
				return (void*)(index*4096+0x40000000);
			}
		}
	}
}

void* phy2virt(void* phy){
	if(phy<0x40000000){
		return phy+0xc0000000;
	}else{
		// TODO: page translate
	}
}

void* virt2phy(void* virt){
	if(virt>0xbfffffff){
		return virt-0xc0000000;
	}else{
		// TODO: page translate
	}
}