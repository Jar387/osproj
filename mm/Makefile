OBJS = init.o buddy.o slab.o mapping.o
INCDIR = ../include

mm.o: $(OBJS)
	$(MERGE) -r $^ -o $@

include ../Makefile.in