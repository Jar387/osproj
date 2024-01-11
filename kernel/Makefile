OBJS = head.o main.o

kernel.o: $(OBJS)
	$(MERGE) -r $^ -o $@

include ../Makefile.in