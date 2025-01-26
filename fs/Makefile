OBJS = vfs.o
INCDIR = ../include

.PHONY: fs.o
fs.o: $(OBJS)
	$(MERGE) -r $(OBJS) -o $@

include ../Makefile.in