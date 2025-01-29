#ifndef RING0_H
#define RING0_H

#define io_wait() \
    __asm__ volatile ("nop; nop\n")

#define outb(port, data) \
    __asm__ volatile ("outb %%al, %%dx"::"a" (data), "d"(port))

#define outb_p(port, data) \
    do { \
        outb(port, data); \
        io_wait(); \
    } while (0)

#define inb(port) ({ \
    unsigned char result; \
    __asm__ volatile ("inb %%dx":"=a" (result):"d"(port)); \
    result; \
})

#define inw(port) ({ \
    unsigned short result; \
    __asm__ volatile ("inw %%dx":"=a" (result):"d"(port)); \
    result; \
})

#define outw(port, data) \
    __asm__ volatile ("outw %%ax, %%dx"::"a" (data), "d"(port))

#define outw_p(port, data) \
    do { \
        outw(port, data); \
        io_wait(); \
    } while (0)

#define inl(port) ({ \
    unsigned int result; \
    __asm__ volatile ("inl %%dx":"=a" (result):"d"(port)); \
    result; \
})

#define outl(port, data) \
    __asm__ volatile ("outl %0, %1" : : "a" (data), "Nd" (port))

#define outl_p(port, data) \
    do { \
        outl(port, data); \
        io_wait(); \
    } while (0)

#define lgdt(gdt) \
    __asm__ volatile ("lgdt (%0)"::"r" (gdt))

#define lidt(idt) \
    __asm__ volatile ("lidt (%0)"::"r" (idt))

#define ltr(tr) \
    __asm__ volatile ("ltr %%ax"::"a" (tr))

#define rdmsr(msr, lo, hi) \
    __asm__ volatile ("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr))

#define wrmsr(msr, lo, hi) \
    __asm__ volatile ("wrmsr" : : "a"(lo), "d"(hi), "c"(msr))

#define flush_segment(cs, ds) \
    __asm__ volatile ("\n" \
                      "pushl %0\n" \
                      "pushl $1f\n" \
                      "lret \n" \
                      "1:\n" \
                      "mov %1, %%eax\n" \
                      "mov %%ax, %%ds\n" \
                      "mov %%ax, %%es\n" \
                      "mov %%ax, %%fs\n" \
                      "mov %%ax, %%gs\n" \
                      "mov %%ax, %%ss\n"::"g" (cs), "g"(ds):"eax")

#define flush_cr3() \
    __asm__ volatile ("\n" \
                      "movl %%cr3, %%eax\n" \
                      "movl %%eax, %%cr3\n":::"eax")

#define hlt() \
    __asm__ volatile ("hlt")

#define lock_kernel() \
    __asm__ volatile ("cli")

#define unlock_kernel() \
    __asm__ volatile ("sti")

#define bp() \
    __asm__ volatile ("xchgw %bx, %bx")

#define nop() \
    __asm__ volatile ("nop")

#endif
