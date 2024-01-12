static inline void outb(unsigned short port, unsigned char data){
	__asm__ volatile ("outb %%al, %%dx"::"a"(data), "d"(port));
}

static inline unsigned char inb(unsigned short port){
	unsigned char result;
	__asm__ volatile ("inb %%dx":"=a"(result): "d"(port));
}

static inline void lgdt(void* gdt){
	__asm__ volatile ("lgdt (%0)"::"r"(gdt));
}

static inline void lidt(void* idt){
	__asm__ volatile ("lidt (%0)"::"r"(idt));
}

static inline void flush_segment(unsigned int cs, unsigned int ds){
	__asm__ volatile (
         "\n"
         "pushl %0\n"
         "pushl $1f\n"
         "lret \n"
        "1:\n"
         "mov %1, %%eax\n"
         "mov %%ax, %%ds\n"
         "mov %%ax, %%es\n"
         "mov %%ax, %%fs\n"
         "mov %%ax, %%gs\n"
         "mov %%ax, %%ss\n"

       :
       : "g" (cs), "g" (ds)
       : "eax");
}

static inline void hlt(){
	__asm__ volatile ("hlt");
}

static inline void cli(){
	__asm__ volatile ("cli");
}

static inline void sti(){
	__asm__ volatile ("sti");
}

static inline void bp(){
	__asm__ volatile ("xchgw %bx, %bx");
}