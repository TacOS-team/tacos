INCLUDES=-I../include/ -Iinclude/
CFLAGS=-W -Wall $(WE) -g -ffreestanding -nostdlib -nostdinc -fno-builtin -finline-functions -I`pwd` -m32 -fexec-charset=iso-8859-1

SRCS = *.c fs/*.c fs/fat/*.c fs/ext2/*.c pci/*.c utils/*.c klibc/*.c
OBJS = boot.o exception_wrappers.o interrupts_wrappers.o kdirent.o kstdio.o kernel.o i8259.o idt.o gdt.o memory.o exception.o interrupts.o scheduler.o kprocess.o pagination.o kpanic.o ksyscall.o vmm.o kmalloc.o fpu.o ksem.o kfcntl.o kunistd.o ksignal.o round_robin.o events.o clock.o i8254.o symtable.o module.o init.o vfs.o dcache.o elf.o pipe.o fs/procfs.o fs/devfs.o pci/pci.o pci/pci_config.o fs/fat/fat.o fs/fat/fat_functions.o fs/fat/fat_outils.o fs/ext2/ext2.o fs/ext2/ext2_functions.o utils/heap.o utils/list.o utils/hashmap.o klibc/string.o klibc/stdlib.o

all: $(OBJS)

boot.o: boot.S
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) -fno-stack-protector

exception_wrappers.o: exception_wrappers.S include/exception.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) -DASM_SOURCE=1 -fno-stack-protector

interrupts_wrappers.o: interrupts_wrappers.S include/interrupts.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) -DASM_SOURCE=1 -fno-stack-protector

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f $(OBJS) .depend

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $^ > .depend
