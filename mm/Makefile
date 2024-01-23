OBJS = memman.o page.o slab.o
INCDIR = ../include

mm.o: $(OBJS)
	$(MERGE) -r $^ -o $@

include ../Makefile.in