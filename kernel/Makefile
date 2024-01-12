OBJS = head.o main.o cpu.o
INCDIR = ../include

kernel.o: $(OBJS)
	$(MERGE) -r $^ -o $@

include ../Makefile.in