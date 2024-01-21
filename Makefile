LIBOBJS = kernel/kernel.o drivers/drivers.o mm/mm.o

.PHONY: kernel.out
kernel.out: Makefile link.ld Makefile.in
	(cd kernel; make)
	(cd drivers; make)
	(cd mm; make)
	$(LD) $(TARGETFLAGS) $(LIBOBJS)  -Xlinker -Map=System.map 

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