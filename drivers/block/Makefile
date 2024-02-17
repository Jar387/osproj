OBJS = ata.o blkdev.o
INCDIR = ../../include

block.o: $(OBJS)
	$(MERGE) -r $^ -o $@

include ../../Makefile.in