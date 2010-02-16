CC=gcc
LD=ld
CFLAGS=-W -Wall -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

all: kernel.bin

kernel.bin: kernel.o boot.o stdio.o i8259.o idt.o mempage.o gdt.o exception.o exception_wrappers.o interrupts.o interrupts_wrappers.o pci.o pci_config.o scheduler.o dummy_process.o keyboard.o linker.ld
	$(LD) -T linker.ld -o kernel.bin boot.o kernel.o stdio.o i8259.o idt.o mempage.o exception_wrappers.o exception.o gdt.o interrupts.o interrupts_wrappers.o pci.o pci_config.o scheduler.o dummy_process.o keyboard.o -melf_i386

kernel.o: kernel.c multiboot.h types.h mempage.h stdio.h gdt.h idt.h scheduler.h dummy_process.h
	$(CC) -o kernel.o -c kernel.c $(CFLAGS)

stdio.o: stdio.c stdio.h ioports.h types.h
	$(CC) -o stdio.o -c stdio.c $(CFLAGS)

boot.o: boot.S
	$(CC) -o boot.o -c boot.S $(CFLAGS) -fno-stack-protector

i8259.o: i8259.c i8259.h types.h
	$(CC) -o i8259.o -c i8259.c $(CFLAGS)

idt.o: idt.c idt.h types.h
	$(CC) -o idt.o -c idt.c $(CFLAGS)

gdt.o: gdt.c gdt.h types.h
	$(CC) -o gdt.o -c gdt.c $(CFLAGS)

mempage.o: mempage.c mempage.h types.h
	$(CC) -o mempage.o -c mempage.c $(CFLAGS)

exception.o: exception.c exception.h types.h idt.h
	$(CC) -o exception.o -c exception.c $(CFLAGS)

exception_wrappers.o: exception_wrappers.S exception.h
	$(CC) -o exception_wrappers.o -c exception_wrappers.S $(CFLAGS) -DASM_SOURCE=1 -fno-stack-protector

interrupts.o: interrupts.c interrupts.h types.h idt.h
	$(CC) -o interrupts.o -c interrupts.c $(CFLAGS)

interrupts_wrappers.o: interrupts_wrappers.S interrupts.h
	$(CC) -o interrupts_wrappers.o -c interrupts_wrappers.S $(CFLAGS) -DASM_SOURCE=1 -fno-stack-protector

scheduler.o: scheduler.c stdio.h types.h scheduler.h
	$(CC) -o scheduler.o -c scheduler.c $(CFLAGS)

dummy_process.o: dummy_process.c stdio.h types.h dummy_process.h
	$(CC) -o dummy_process.o -c dummy_process.c $(CFLAGS)

pci.o : pci.c
	$(CC) -o pci.o -c pci.c $(CFLAGS)
pci_config.o : pci_config.o
	$(CC) -o pci_config.o -c pci_config.c $(CFLAGS)

keyboard.o : keyboard.c
	$(CC) -o $@ -c $^ $(CFLAGS)


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
	rm -f *.o *.bin *.img
