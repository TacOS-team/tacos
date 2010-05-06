export MAKE=make
export CC=@echo "   CC   $$@" && gcc
export LD=@echo "   LD   $$@" && ld
export AR=@echo "   AR   $$@" && ar
export CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
LDFLAGS=-Llib/
LDLIBS=-lc -lpci -lclock -lutils -ldrivers -z nodefaultlib -lsystem

SUBDIRS = kernel libc utils drivers pci clock system

all: kernel.bin
	@for i in $(SUBDIRS); do \
		$(MAKE) -C $$i; \
	done
	readelf --syms kernel.bin | awk '{print $2 " " $8}' > symbols

kernel.bin: force_look 
	$(MAKE) -C kernel
	$(LD) -T linker.ld -o kernel.bin kernel/boot.o kernel/kernel.o kernel/i8259.o kernel/idt.o kernel/pagination.o kernel/memory.o kernel/exception_wrappers.o kernel/exception.o kernel/gdt.o kernel/interrupts.o kernel/interrupts_wrappers.o kernel/scheduler.o kernel/dummy_process.o kernel/process.o kernel/kpanic.o kernel/ksyscall.o kernel/vmm.o kernel/kmalloc.o kernel/fat.o kernel/fpu.o kernel/vm86.o kernel/shell.o kernel/shell_utils.o -melf_i386 $(LDFLAGS) $(LDLIBS)

force_look:
	@true

core.img: img

img: all
	@echo "drive v: file=\"`pwd`/core.img\" 1.44M filter" > mtoolsrc
	@gzip -dc < grub.img.gz > core.img
	MTOOLSRC=mtoolsrc mcopy menu.txt v:/boot/grub/
	MTOOLSRC=mtoolsrc mmd v:/system
	MTOOLSRC=mtoolsrc mcopy kernel.bin v:/system/
	@rm mtoolsrc

runqemu: core.img
	qemu -fda core.img -m 8

runqemugdb: core.img
	qemu -fda core.img -m 8 -s -S

runbochs: core.img
	BOCHSRC=bochsrc bochs

clean:
	@for i in $(SUBDIRS); do \
		$(MAKE) -C $$i clean; \
	done
	@rm -f *.o *.bin *.img

depend:
	@for i in $(SUBDIRS); do \
		$(MAKE) -C $$i depend; \
	done
