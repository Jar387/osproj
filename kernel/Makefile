OBJS = head.o main.o
INCDIR = ../include

kernel.o: $(OBJS)
	$(MERGE) -r $^ -o $@

include ../Makefile.in