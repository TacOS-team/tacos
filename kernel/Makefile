INCLUDE=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = dummy_process.c fat.c gdt.c idt.c kernel.c kpanic.c mbr.c pagination.c scheduler.c shell_utils.c vmm.c exception.c fpu.c i8259.c interrupts.c kmalloc.c ksyscall.c memory.c process.c shell.c vm86.c

all: boot.o exception_wrappers.o interrupts_wrappers.o kernel.o i8259.o idt.o gdt.o memory.o exception.o interrupts.o scheduler.o dummy_process.o process.o pagination.o kpanic.o ksyscall.o vmm.o kmalloc.o fat.o fpu.o vm86.o shell.o shell_utils.o

boot.o: boot.S
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE) -fno-stack-protector

exception_wrappers.o: exception_wrappers.S ../include/exception.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE) -DASM_SOURCE=1 -fno-stack-protector

interrupts_wrappers.o: interrupts_wrappers.S ../include/interrupts.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE) -DASM_SOURCE=1 -fno-stack-protector

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE) 

clean:
	rm -f *.o

depend: $(SRCS)
	makedepend -Y $(INCLUDE) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

dummy_process.o: ../include/stdio.h ../include/libio.h ../include/fcntl.h
dummy_process.o: ../include/types.h ../include/stdarg.h
dummy_process.o: include/dummy_process.h
fat.o: ../include/stdio.h ../include/libio.h ../include/fcntl.h
fat.o: ../include/types.h ../include/stdarg.h ../include/floppy.h
fat.o: ../include/string.h include/fat.h ../include/debug.h
gdt.o: ../include/types.h ../include/memory.h include/gdt.h
idt.o: ../include/types.h include/idt.h
kernel.o: include/multiboot.h include/idt.h ../include/types.h
kernel.o: include/i8259.h ../include/stdio.h ../include/libio.h
kernel.o: ../include/fcntl.h ../include/stdarg.h ../include/stdlib.h
kernel.o: ../include/unistd.h ../include/string.h ../include/memory.h
kernel.o: ../include/pagination.h include/gdt.h ../include/exception.h
kernel.o: ../include/interrupts.h ../include/pci.h ../include/pci_config.h
kernel.o: ../include/pci_types.h include/scheduler.h ../include/time.h
kernel.o: include/dummy_process.h ../include/keyboard.h ../include/mouse.h
kernel.o: ../include/events.h ../include/floppy.h include/kpanic.h
kernel.o: ../include/process.h ../include/kmalloc.h include/vmm.h
kernel.o: ../include/ioports.h include/msr.h include/fat.h include/fpu.h
kernel.o: include/vm86.h ../include/beeper.h ../include/debug.h
kernel.o: include/shell.h include/ksyscall.h ../include/syscall.h
kpanic.o: ../include/stdio.h ../include/libio.h ../include/fcntl.h
kpanic.o: ../include/types.h ../include/stdarg.h ../include/exception.h
kpanic.o: include/kpanic.h
mbr.o: ../include/stdio.h ../include/libio.h ../include/fcntl.h
mbr.o: ../include/types.h ../include/stdarg.h ../include/floppy.h
mbr.o: include/mbr.h
pagination.o: ../include/types.h ../include/memory.h ../include/pagination.h
scheduler.o: ../include/stdio.h ../include/libio.h ../include/fcntl.h
scheduler.o: ../include/types.h ../include/stdarg.h ../include/process.h
scheduler.o: ../include/pagination.h include/scheduler.h ../include/events.h
scheduler.o: ../include/time.h ../include/clock.h ../include/ioports.h
scheduler.o: include/gdt.h include/i8259.h ../include/syscall.h
scheduler.o: ../include/debug.h
shell_utils.o: ../include/types.h ../include/stdio.h ../include/libio.h
shell_utils.o: ../include/fcntl.h ../include/stdarg.h ../include/stdlib.h
shell_utils.o: ../include/string.h include/shell_utils.h
vmm.o: ../include/memory.h ../include/types.h ../include/pagination.h
vmm.o: include/vmm.h ../include/stdio.h ../include/libio.h ../include/fcntl.h
vmm.o: ../include/stdarg.h
exception.o: ../include/types.h include/idt.h ../include/exception.h
fpu.o: ../include/stdio.h ../include/libio.h ../include/fcntl.h
fpu.o: ../include/types.h ../include/stdarg.h include/fpu.h
fpu.o: ../include/interrupts.h
i8259.o: ../include/types.h ../include/ioports.h
interrupts.o: ../include/types.h include/idt.h include/i8259.h
interrupts.o: ../include/interrupts.h
kmalloc.o: ../include/types.h ../include/stdio.h ../include/libio.h
kmalloc.o: ../include/fcntl.h ../include/stdarg.h ../include/heap.h
kmalloc.o: include/vmm.h ../include/kmalloc.h
ksyscall.o: include/msr.h ../include/stdio.h ../include/libio.h
ksyscall.o: ../include/fcntl.h ../include/types.h ../include/stdarg.h
ksyscall.o: ../include/string.h ../include/interrupts.h include/ksyscall.h
ksyscall.o: ../include/syscall.h
memory.o: ../include/types.h ../include/stdio.h ../include/libio.h
memory.o: ../include/fcntl.h ../include/stdarg.h ../include/memory.h
process.o: ../include/types.h ../include/stdlib.h ../include/stdio.h
process.o: ../include/libio.h ../include/fcntl.h ../include/stdarg.h
process.o: ../include/string.h include/gdt.h ../include/process.h
process.o: ../include/pagination.h ../include/kmalloc.h ../include/syscall.h
process.o: ../include/time.h ../include/debug.h
shell.o: include/fat.h ../include/types.h ../include/fcntl.h
shell.o: ../include/floppy.h ../include/kmalloc.h ../include/memory.h
shell.o: ../include/mouse.h ../include/pci.h ../include/process.h
shell.o: ../include/pagination.h ../include/libio.h ../include/stdio.h
shell.o: ../include/stdarg.h ../include/stdlib.h ../include/string.h
shell.o: ../include/time.h include/shell_utils.h ../include/debug.h
vm86.o: ../include/debug.h include/gdt.h ../include/types.h
