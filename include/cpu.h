#ifndef CPU_H
#define CPU_H

// 80686 cpu internal data structure
// pae PSE PG PE nx MMX xmm sse

// segment part

#define MAGIC 0xc

#define R 0x1
#define RW 0x2
#define X 0x8
#define RX 0xa

#define RING0 0x0
#define RING3 0x3

#define SYS 0x0
#define RWX 0x1

#define TASK_GATE 0x5
#define INTR_GATE 0xe
#define TRAP_GATE 0xf

#define PDT_BASE 0xc0000000
#define KERNEL_BASE 0xc0000000
#define PDT_SIZE 0x1000 // 4KB

struct xdtr{ // '?dtr'(gdtr, idtr, ldtr, tr...)
	unsigned short limit;
	void* addr;
}__attribute__((packed));

struct segment_desc{ // in gdt and ldt
	unsigned short limitlow;
	unsigned short baselow;
	unsigned char basemid;
	unsigned char type : 4;
	unsigned char sysflag : 1;
	unsigned char dpl : 2;
	unsigned char present : 1;
	unsigned char limithigh : 4;
	unsigned char magic : 4;
	unsigned char basehigh : 4;
}__attribute__((packed));

struct gate_desc{ // in idt and tr
	unsigned short addrlow;
	unsigned short segment;
	unsigned char reserve0;
	unsigned char type : 4;
	unsigned char reserve1 : 1;
	unsigned char dpl : 2;
	unsigned char present : 1;
	unsigned short addrhigh;
}__attribute__((packed));

extern void intr_stub();

void arch_init();
void register_intr(unsigned char num, void* ISR,unsigned short segment , unsigned char dpl);
void register_trap(unsigned char num, void* ISR,unsigned short segment , unsigned char dpl);

static inline void set_int_gate(unsigned char num, void* ISR){
	register_intr(num, ISR, 1<<3, 0);
}

static inline void set_system_gate(unsigned char num, void* ISR){
	register_intr(num, ISR, 1<<3, 3);
}

// paging part

typedef struct{
	unsigned char present : 1;
	unsigned char rw : 1;
	unsigned char us : 1;
	unsigned char pwt : 1; // write through
	unsigned char pcd : 1; // disable cache
	unsigned char accessed : 1;
	unsigned char dirty : 1;
	unsigned char pse : 1;
	unsigned char global : 1;
	unsigned char avl : 3;
	unsigned short reserved : 10;
	unsigned short addr : 10;
}__attribute__((packed)) huge_pde_t;

typedef struct{
	unsigned char present : 1;
	unsigned char rw : 1;
	unsigned char us : 1;
	unsigned char pwt : 1; // write through
	unsigned char pcd : 1; // disable cache
	unsigned char accessed : 1;
	unsigned char dirty : 1;
	unsigned char pse : 1;
	unsigned char avl : 4;
	unsigned int addr : 20;
}__attribute__((packed)) pde_t;

typedef struct{
	unsigned char present : 1;
	unsigned char rw : 1;
	unsigned char us : 1;
	unsigned char pwt : 1; // write through
	unsigned char pcd : 1; // disable cache
	unsigned char accessed : 1;
	unsigned char dirty : 1;
	unsigned char reserved : 1;
	unsigned char global : 1;
	unsigned char avl : 3;
	unsigned int addr : 20;
}__attribute__((packed)) pte_t;

#endif