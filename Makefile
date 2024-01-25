LIBOBJS = kernel/kernel.o drivers/drivers.o mm/mm.o lib/lib.o

.PHONY: kernel.out
kernel.out: Makefile link.ld Makefile.in
	(cd kernel; make)
	(cd drivers; make)
	(cd mm; make)
	(cd lib; make)
	$(LD) $(TARGETFLAGS) $(LIBOBJS)  -Xlinker -Map=System.map 

.PHONY: run
run: kernel.out
	cp kernel.out mnt/
	sync
	qemu-system-i386 -hda disk.img -no-reboot -no-shutdown

.PHONY: debug
debug: kernel.out
	cp kernel.out mnt/
	sync
	bochs


.PHONY: clean
clean:
	-rm kernel.out
	-make -C kernel subclean
	-make -C mm subclean
	-make -C drivers subclean
	-make -C lib subclean

include ./Makefile.in