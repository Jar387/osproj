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

struct xdtr{
	unsigned short limit;
	void* addr;
}__attribute__((packed));

struct segment_desc{
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

struct gate_desc{
	unsigned short addrlow;
	unsigned short segment;
	unsigned char reserve0;
	unsigned char type : 4;
	unsigned char reserve1;
	unsigned char dpl : 2;
	unsigned char present : 1;
	unsigned short addrhigh;
}__attribute__((packed));

void arch_init();
void register_intr(unsigned char num, void* ISR,unsigned short segment , unsigned char dpl);
void register_trap(unsigned char num, void* ISR,unsigned short segment , unsigned char dpl);