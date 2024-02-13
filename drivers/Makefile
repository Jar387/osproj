OBJS = char/char.o block/block.o
INCDIR = ../include

.PHONY: drivers.o
drivers.o:
	(cd char; make)
	(cd block; make)
	$(MERGE) -r $(OBJS) -o $@

include ../Makefile.in