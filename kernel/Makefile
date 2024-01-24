OBJS = head.o main.o cpu.o interrupt.o printk.o panic.o isr.o
INCDIR = ../include

kernel.o: $(OBJS)
	$(MERGE) -r $^ -o $@

include ../Makefile.in