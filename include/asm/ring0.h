#ifndef RING0_H
#define RING0_H

static inline void io_wait(){
    __asm__ volatile ("nop; nop\n");
}

static inline void outb(unsigned short port, unsigned char data){
	__asm__ volatile ("outb %%al, %%dx"::"a"(data), "d"(port));
}

static inline void outb_p(unsigned short port, unsigned char data){
    outb(port, data);
    io_wait();
}

static inline unsigned char inb(unsigned short port){
    unsigned char result;
    __asm__ volatile ("inb %%dx":"=a"(result): "d"(port));
}

static inline unsigned short inw(unsigned short port){
	unsigned short result;
	__asm__ volatile ("inw %%dx":"=a"(result): "d"(port));
}

static inline void outw(unsigned short port, unsigned short data){
    __asm__ volatile ("outw %%ax, %%dx"::"a"(data), "d"(port));
}

static inline void outw_p(unsigned short port, unsigned short data){
    outw(port, data);
    io_wait();
}

static inline void lgdt(void* gdt){
	__asm__ volatile ("lgdt (%0)"::"r"(gdt));
}

static inline void lidt(void* idt){
	__asm__ volatile ("lidt (%0)"::"r"(idt));
}

static inline void ltr(unsigned int tr){ // tr is 16-bit
    __asm__ volatile ("ltr %%ax"::"a"(tr));
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

static inline void flush_cr3(){
    __asm__ volatile (
        "\n"
        "movl %%cr3, %%eax\n"
        "movl %%eax, %%cr3\n"
        :
        :
        : "eax"
        );
}

static inline void hlt(){
	__asm__ volatile ("hlt");
}

static inline void lock_kernel(){
	__asm__ volatile ("cli");
}

static inline void unlock_kernel(){
	__asm__ volatile ("sti");
}

#endif