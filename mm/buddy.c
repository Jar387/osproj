#include <buddy.h>
#include <stddef.h>

static page_t* memmap_kernel;
static page_t* memmap_user;
static page_t* memmap_end;

static unsigned int kernel_mem_pages;
static unsigned int user_mem_pages;

static void* phy2virt(void* phy){
	if((unsigned int)phy<0x40000000){
		return phy+0xc0000000;
	}else{
		return NULL;
	}
}

static void* virt2phy(void* virt){
	if((unsigned int)virt>0xbfffffff){
		return virt-0xc0000000;
	}else{
		return NULL;
	}
}

void page_init(unsigned int total_mem){
	kernel_mem_pages = total_mem>>4;
	user_mem_pages = (total_mem>>4)*3;
	// build physical mem track bytemap
	// kernel uses low physical mem 0x0-0x3fffffff that mapped to 0xc0000000-0xffffffff
	// user uses high physical mem 0x40000000-0xffffffff that mapped to 0x0-0xbfffffff
	// address space is dynamicly changed within actually physical mem size
	memmap_kernel = (page_t*)KERNEL_END+0xc0000000;
	memmap_user = (page_t*)(KERNEL_END+kernel_mem_pages)+0xc0000000;
	memmap_end = memmap_user+user_mem_pages;
	unsigned int kernel_reserved_pages = (((unsigned int)memmap_end-0xc0000000)>>12)+1;
	page_t* ptr = memmap_kernel;
	for(int i=0;i<kernel_reserved_pages;i++){
		ptr->status = MEM_USED;
		ptr++;
	}
	for(;(unsigned int)ptr<(unsigned int)memmap_end;ptr++){
		ptr->status = MEM_UNUSED;
	}
}

void* palloc(int zone, unsigned int size){
	page_t* ptr;
	page_t* head_ptr;
	unsigned int searchlen;
	unsigned int index=0;
	unsigned int curr_size = size;
	if(zone!=ZONE_USER&&zone!=ZONE_KERNEL){
		return NULL;
	}
	if(zone==ZONE_KERNEL){
		ptr=memmap_kernel;
		searchlen=kernel_mem_pages;
	}if(zone==ZONE_USER){
		ptr=memmap_user;
		searchlen=user_mem_pages;
	}
	head_ptr = ptr;
	for(;index<searchlen;index++){
		if(ptr[index].status==MEM_UNUSED){
			curr_size--;
			if(curr_size==0){
				for(int i=0;i<size;i++){
					head_ptr[i].status = MEM_USED;
				}
				if(zone==ZONE_KERNEL){
					return phy2virt((void*)((index-size)<<12));
				}
				if(zone==ZONE_USER){
					return (void*)((index-size)<<12);
				}
			}
			continue;
		}else{
			head_ptr++;
			curr_size = size;
		}
	}
	return NULL;
}

void pfree(void* addr, unsigned int len, unsigned int zone){
	page_t* ptr;
	if(zone==ZONE_USER){
		// user space
		ptr = (page_t*)((unsigned int)memmap_user+((unsigned int)addr>>12));
	}else{ // kernel space
		ptr = (page_t*)((unsigned int)memmap_kernel+((unsigned int)addr>>12));
	}
	for(int i=1;i<len;i++){
		ptr[i].status = MEM_UNUSED;
	}
}