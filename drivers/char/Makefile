OBJS = vga_text.o chardev.o
INCDIR = ../../include

char.o: $(OBJS)
	$(MERGE) -r $^ -o $@

include ../../Makefile.in