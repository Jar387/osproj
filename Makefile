LIBOBJS = kernel/kernel.o

kernel.out: Makefile link.ld Makefile.in
	make -C kernel kernel.o
	$(LD) $(TARGETFLAGS) $(LIBOBJS)

.PHONY: run
run: kernel.out
	cp kernel.out mnt/
	sync
	qemu-system-i386 -hda disk.img

.PHONY: debug
debug: kernel.out
	cp kernel.out mnt/
	sync
	bochs


.PHONY: clean
clean:
	-rm kernel.out
	make -C kernel subclean

include ./Makefile.in