#include <printk.h>

static void
dump_cpu()
{
	unsigned int eax, ebx, ecx, edx, esi, edi, esp, ebp;
	unsigned int eip, eflags;
	unsigned int cs, ds, es, fs, gs, ss;
	unsigned int cr0, cr2, pdbr;
	__asm__ volatile ("\n"
			  "nop\n":"=a" (eax), "=b"(ebx), "=c"(ecx), "=d"(edx),
			  "=D"(edi), "=S"(esi)
			  :);
	__asm__ volatile ("\n"
			  "movl %%esp, %%eax\n"
			  "movl %%ebp, %%ebx\n"
			  "pushfl; popl %%ecx\n"
			  "call 1f\n"
			  "1:\n"
			  "popl %%edx\n"
			  "movl %%cr3, %%esi\n"
			  "movl %%cr0, %%edi\n":"=a" (esp), "=b"(ebp),
			  "=c"(eflags), "=d"(eip), "=S"(pdbr), "=D"(cr0)
			  :);
	__asm__ volatile ("\n" "movl %%cr2, %%eax\n":"=a" (cr2)
			  :);
	__asm__ volatile ("\n"
			  "mov %%cs, %%eax\n"
			  "mov %%ds, %%ebx\n"
			  "mov %%es, %%ecx\n"
			  "mov %%fs, %%edx\n"
			  "mov %%gs, %%edi\n"
			  "mov %%ss, %%esi\n":"=a" (cs), "=b"(ds), "=c"(es),
			  "=d"(fs), "=D"(gs), "=S"(ss)
			  :);
	printk
	    ("eax=%p, ebx=%p, ecx=%p, edx=%p\nedi=%p, esi=%p, ebp=%p, esp=%p\n\
cs=%p, ss=%p, ds=%p, es=%p, fs=%p, gs=%p\n\
current eip=%p, cr0=%p, cr2=%p, cr3=%p\n\
-----------------------------------CALL STACK-----------------------------------", eax, ebx, ecx, edx, edi, esi, ebp, esp, cs, ss, ds, es, fs, gs, eip, cr0, cr2, pdbr);
	unsigned int *stack = (unsigned int *) esp;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 8; j++) {
			printk("%p ", *stack);
			stack--;
		}
		if (i == 25 - 7) {
			break;
		}
		printk("\n");
	}
}

__attribute__ ((noreturn))
void
panic(const char *reason)
{
	printk("Kernel Panic: %s\n", reason);
	dump_cpu();
	for (;;) ;
}
