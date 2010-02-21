export MAKE=make
export CC=gcc
export LD=ld
export CFLAGS=-W -Wall -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
LDFLAGS=-Llib/
LDLIBS=-lc -lpci -lkeyboard -lclock

all: libc pci keyboard clock kernel.bin

kernel.bin: force_look 
	$(MAKE) -C kernel
	$(LD) -T linker.ld -o kernel.bin kernel/boot.o kernel/kernel.o kernel/i8259.o kernel/idt.o kernel/mempage.o kernel/exception_wrappers.o kernel/exception.o kernel/gdt.o kernel/interrupts.o kernel/interrupts_wrappers.o kernel/scheduler.o kernel/dummy_process.o -melf_i386 $(LDFLAGS) $(LDLIBS)

keyboard: force_look 
	$(MAKE) -C $@ 

libc: force_look 
	$(MAKE) -C $@ 

pci: force_look 
	$(MAKE) -C $@

clock: force_look 
	$(MAKE) -C $@

force_look:
	@true

img:
	echo "drive v: file=\"`pwd`/core.img\" 1.44M filter" > mtoolsrc
	gzip -dc < grub.img.gz > core.img
	MTOOLSRC=mtoolsrc mcopy menu.txt v:/boot/grub/
	MTOOLSRC=mtoolsrc mmd v:/system
	MTOOLSRC=mtoolsrc mcopy kernel.bin v:/system/
	rm mtoolsrc

runqemu:
	qemu -fda core.img -m 32

runbochs:
	BOCHSRC=bochsrc bochs

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C libc clean
	$(MAKE) -C pci clean
	$(MAKE) -C keyboard clean
	rm -f *.o *.bin *.img

