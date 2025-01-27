#include <asm/ring0.h>
#include <cpu.h>
#include <interrupt.h>
#include <pic.h>
#include <apic.h>
#include <cpuid.h>
#include <mm/slab.h>
#include <panic.h>

static segment_desc_t gdt[6];
static xdtr_t gdtr;

static gate_desc_t idt[256];
static xdtr_t idtr;

// star feat if mand
static char *cpuid_feat_c[] =
    { "sse3", "pclmul", "dtes64", "monitor", "ds_cpl", "vmx", "smx", "est",
	"tm2", "ssse3", "cid", "sdbg", "fma", "cx16", "xtpr", "pdcm", "pcid",
	    "dca", "sse4_1",
	"sse4_2", "x2apic", "movbe", "popcnt", "tsc", "aes", "xsave", "osxsave",
	    "avx", "f16c",
	"rdrand", "hypervisor"
};

static char *cpuid_feat_d[] =
    { "fpu", "vme", "de", "*pse", "tsc", "*msr", "*pae", "mce", "cx8", "*apic",
	"sep", "mtrr", "pge", "mca", "cmov", "pat", "pse36", "psn", "clflush",
	    "ds", "acpi", "mmx",
	"fxsr", "sse", "sse2", "ss", "htt", "tm", "ia64", "pbe"
};

static void
setup_gdt()
{
	gdtr.limit = 6 * sizeof (segment_desc_t);
	gdtr.addr = &gdt;
	int *ptr = (int *) gdt;
	*ptr = 0;
	ptr++;
	*ptr = 0;
	for (int i = 1; i < 5; i++) {
		gdt[i].limitlow = 0xffff;
		gdt[i].baselow = 0;
		gdt[i].basemid = 0;
		gdt[i].sysflag = RWX;
		gdt[i].present = 1;
		gdt[i].limithigh = 0xf;
		gdt[i].magic = MAGIC;
		gdt[i].basehigh = 0;
	}
	gdt[1].type = RX;
	gdt[2].type = RW;
	gdt[3].type = RX;
	gdt[4].type = RW;
	gdt[1].dpl = RING0;
	gdt[2].dpl = RING0;
	gdt[3].dpl = RING3;
	gdt[4].dpl = RING3;
	lgdt(&gdtr);
	flush_segment(1 << 3, 2 << 3);
}

static void
setup_idt()
{
	idtr.limit = 256 * sizeof (gate_desc_t);
	idtr.addr = &idt;
	lidt(&idtr);
}

static void
print_uppercase(char *str)
{
	char buf[2];
	buf[1] = '\0';
	char *p = str;
	while (*p != '\0') {
		if (*p >= 'a' && *p <= 'z') {
			buf[0] = *p - 32;
		} else {
			buf[0] = *p;
		}
		printk("%s", buf);
		p++;
	}
}

static void
check_cpu()
{
	// print vendor
	int vendor_buf[4];
	unsigned int ecx, unused, edx;
	__get_cpuid(0, &unused, vendor_buf, vendor_buf + 2, vendor_buf + 1);
	vendor_buf[3] = 0;
	printk("vendor_id: %s\n", (char *) vendor_buf);
	printk("flags: ");
	__get_cpuid(1, &unused, &unused, &ecx, &edx);
	for (int i = 0; i < (sizeof (cpuid_feat_c) / sizeof (char *)); i++) {
		if ((ecx & (1 << i)) != (1 << i)) {
			if (cpuid_feat_c[i][0] == '*') {
				printk("\ncpu lack of feature %s\n",
				       cpuid_feat_c[i] + 1);
				panic("unsupported cpu");
			}
			printk("%s ", cpuid_feat_c[i]);
		} else {
			if (cpuid_feat_c[i][0] == '*') {
				print_uppercase(cpuid_feat_c[i] + 1);
				printk(" ");
			} else {
				print_uppercase(cpuid_feat_c[i]);
				printk(" ");
			}
		}
	}
	printk(" ");
	for (int i = 0; i < (sizeof (cpuid_feat_d) / sizeof (char *)); i++) {
		if ((edx & (1 << i)) != (1 << i)) {
			if (cpuid_feat_d[i][0] == '*') {
				printk("\ncpu lack of feature %s\n",
				       cpuid_feat_d[i] + 1);
				panic("unsupported cpu");
			}
			printk("%s ", cpuid_feat_d[i]);
		} else {
			if (cpuid_feat_d[i][0] == '*') {
				print_uppercase(cpuid_feat_d[i] + 1);
				printk(" ");
			} else {
				print_uppercase(cpuid_feat_d[i]);
				printk(" ");
			}
		}
	}
	printk("\n");
}

void
register_intr(unsigned char num, void *ISR, unsigned short segment,
	      unsigned char dpl)
{
	idt[num].addrlow = (unsigned short) ((unsigned int) ISR & 0xffff);
	idt[num].segment = segment;
	idt[num].reserve0 = 0;
	idt[num].type = INTR_GATE;
	idt[num].dpl = dpl;
	idt[num].present = 1;
	idt[num].addrhigh = (unsigned short) (((int) ISR >> 16) & 0xffff);
}

void
register_trap(unsigned char num, void *ISR, unsigned short segment,
	      unsigned char dpl)
{
	idt[num].addrlow = (unsigned short) ((unsigned int) ISR & 0xffff);
	idt[num].segment = segment;
	idt[num].reserve0 = 0;
	idt[num].type = TRAP_GATE;
	idt[num].dpl = dpl;
	idt[num].present = 1;
	idt[num].addrhigh = (unsigned short) (((int) ISR >> 16) & 0xffff);
}

void
arch_init()
{
	check_cpu();
	setup_gdt();
	setup_idt();
	set_int_gate(0, &divide_error);
	set_int_gate(1, &debug);
	set_int_gate(2, &nmi);
	set_system_gate(3, &int3);
	set_system_gate(4, &overflow);
	set_system_gate(5, &bounds);
	set_int_gate(6, &opcode);
	set_int_gate(7, &device_not_present);
	set_int_gate(8, &double_fault);
	set_int_gate(9, &i387_overrun);
	set_int_gate(10, &illegal_tss);
	set_int_gate(11, &illegal_segment);
	set_int_gate(12, &stack_overflow);
	set_int_gate(13, &general_protect);
	set_int_gate(14, &page_fault);
	for (int i = 15; i <= 0xff; i++) {
		set_int_gate(i, &reserved);
	}
	apic_init();
	init_8259();
}
