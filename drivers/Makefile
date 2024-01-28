OBJS = char/char.o
INCDIR = ../include

.PHONY: drivers.o
drivers.o:
	(cd char; make)
	$(MERGE) -r $(OBJS) -o $@

include ../Makefile.in