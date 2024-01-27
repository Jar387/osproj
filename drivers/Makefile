OBJS = char/char.o
INCDIR = ../include

drivers.o:
	(cd char; make)
	$(MERGE) -r $(OBJS) -o $@

include ../Makefile.in