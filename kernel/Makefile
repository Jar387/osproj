OBJS = head.o main.o cpu.o interrupt.o printk.o panic.o isr.o time.o apic.o acpi.o pic.o sched.o
INCDIR = ../include

kernel.o: $(OBJS)
	$(MERGE) -r $^ -o $@

include ../Makefile.in