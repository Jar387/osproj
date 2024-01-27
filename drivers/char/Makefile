OBJS = vga_console.o chardev.o
INCDIR = ../../include

char.o: $(OBJS)
	$(MERGE) -r $^ -o $@

include ../../Makefile.in