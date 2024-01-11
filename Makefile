LIBOBJS = kernel/kernel.o

kernel.out: Makefile link.ld Makefile.in
	make -C kernel kernel.o
	$(LD) $(TARGETFLAGS) $(LIBOBJS)

.PHONY: run
run: kernel.out
	cp kernel.out mnt/boot
	sync
	qemu-system-i386 -hda rawImage

.PHONY: debug
debug: kernel.out
	cp kernel.out mnt/boot
	sync
	bochs


.PHONY: clean
clean:
	-rm kernel.out
	make -C kernel subclean

include ./Makefile.in