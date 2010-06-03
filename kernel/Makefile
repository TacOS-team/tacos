INCLUDES=-I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: boot.o exception_wrappers.o interrupts_wrappers.o kernel.o i8259.o idt.o gdt.o memory.o exception.o interrupts.o scheduler.o dummy_process.o process.o pagination.o kpanic.o ksyscall.o vmm.o kmalloc.o fat.o fpu.o vm86.o ksem.o

boot.o: boot.S
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) -fno-stack-protector

exception_wrappers.o: exception_wrappers.S include/exception.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) -DASM_SOURCE=1 -fno-stack-protector

interrupts_wrappers.o: interrupts_wrappers.S ../include/interrupts.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) -DASM_SOURCE=1 -fno-stack-protector

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

dummy_process.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
dummy_process.o: ../include/libio.h ../include/stdarg.h
dummy_process.o: include/dummy_process.h
exception.o: ../include/types.h include/idt.h include/exception.h
fat.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
fat.o: ../include/libio.h ../include/stdarg.h ../include/floppy.h
fat.o: ../include/string.h include/fat.h ../include/debug.h
fpu.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
fpu.o: ../include/libio.h ../include/stdarg.h include/fpu.h
fpu.o: ../include/interrupts.h
gdt.o: ../include/types.h ../include/memory.h include/gdt.h
i8259.o: ../include/types.h ../include/ioports.h
idt.o: ../include/types.h include/idt.h
interrupts.o: ../include/types.h include/idt.h include/i8259.h
interrupts.o: ../include/interrupts.h
kernel.o: include/multiboot.h include/idt.h ../include/types.h
kernel.o: include/i8259.h ../include/stdio.h ../include/fcntl.h
kernel.o: ../include/libio.h ../include/stdarg.h ../include/stdlib.h
kernel.o: ../include/unistd.h ../include/string.h ../include/memory.h
kernel.o: include/pagination.h include/gdt.h include/exception.h
kernel.o: ../include/interrupts.h ../include/pci.h ../include/pci_config.h
kernel.o: ../include/pci_types.h include/scheduler.h ../include/time.h
kernel.o: include/dummy_process.h ../include/keyboard.h ../include/mouse.h
kernel.o: ../include/events.h ../include/floppy.h include/kpanic.h
kernel.o: ../include/process.h ../include/kmalloc.h include/vmm.h
kernel.o: ../include/ioports.h include/msr.h include/fat.h include/fpu.h
kernel.o: include/vm86.h ../include/beeper.h ../include/debug.h
kernel.o: ../include/shell.h include/ksyscall.h ../include/syscall.h
kernel.o: include/ksem.h ../include/video.h
kmalloc.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
kmalloc.o: ../include/libio.h ../include/stdarg.h ../include/heap.h
kmalloc.o: include/vmm.h ../include/kmalloc.h ../include/video.h
kpanic.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
kpanic.o: ../include/libio.h ../include/stdarg.h include/exception.h
kpanic.o: include/kpanic.h
ksem.o: include/ksem.h ../include/types.h include/ksyscall.h
ksem.o: ../include/stdio.h ../include/fcntl.h ../include/libio.h
ksem.o: ../include/stdarg.h ../include/kmalloc.h
ksyscall.o: include/msr.h ../include/stdio.h ../include/fcntl.h
ksyscall.o: ../include/types.h ../include/libio.h ../include/stdarg.h
ksyscall.o: ../include/string.h ../include/interrupts.h include/ksyscall.h
ksyscall.o: ../include/syscall.h
mbr.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
mbr.o: ../include/libio.h ../include/stdarg.h ../include/floppy.h
mbr.o: include/mbr.h
memory.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
memory.o: ../include/libio.h ../include/stdarg.h ../include/memory.h
pagination.o: ../include/types.h ../include/memory.h include/pagination.h
process.o: ../include/types.h ../include/stdlib.h ../include/stdio.h
process.o: ../include/fcntl.h ../include/libio.h ../include/stdarg.h
process.o: ../include/string.h ../include/video.h include/gdt.h
process.o: ../include/process.h ../include/kmalloc.h ../include/syscall.h
process.o: ../include/time.h ../include/debug.h
scheduler.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
scheduler.o: ../include/libio.h ../include/stdarg.h ../include/process.h
scheduler.o: include/scheduler.h ../include/events.h ../include/time.h
scheduler.o: ../include/clock.h ../include/ioports.h include/gdt.h
scheduler.o: include/i8259.h ../include/syscall.h ../include/debug.h
vm86.o: ../include/debug.h include/gdt.h ../include/types.h
vmm.o: ../include/memory.h ../include/types.h include/pagination.h
vmm.o: include/vmm.h ../include/stdio.h ../include/fcntl.h ../include/libio.h
vmm.o: ../include/stdarg.h ../include/video.h
