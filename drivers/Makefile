OBJS = char/char.o
INCDIR = ../include

drivers.o:
	make -C char
	$(MERGE) -r $(OBJS) -o $@

include ../Makefile.in