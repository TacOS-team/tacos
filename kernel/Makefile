INCLUDES=-I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: boot.o exception_wrappers.o interrupts_wrappers.o kernel.o i8259.o idt.o gdt.o memory.o exception.o interrupts.o scheduler.o process.o pagination.o kpanic.o ksyscall.o vmm.o kmalloc.o fat.o fpu.o vm86.o ksem.o elf.o kfcntl.o

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

elf.o: ../include/elf.h ../include/stdio.h ../include/fcntl.h
elf.o: ../include/types.h ../include/libio.h ../include/kfcntl.h
elf.o: ../include/stdarg.h ../include/string.h
exception.o: ../include/types.h include/idt.h include/exception.h
fat.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
fat.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
fat.o: ../include/floppy.h ../include/string.h ../include/fat.h
fat.o: ../include/debug.h
fpu.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
fpu.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
fpu.o: include/fpu.h ../include/interrupts.h
gdt.o: ../include/types.h ../include/memory.h include/gdt.h
i8259.o: ../include/types.h ../include/ioports.h
idt.o: ../include/types.h include/idt.h
interrupts.o: ../include/types.h include/idt.h include/i8259.h
interrupts.o: ../include/interrupts.h
kernel.o: include/multiboot.h include/idt.h ../include/types.h
kernel.o: include/i8259.h ../include/stdio.h ../include/fcntl.h
kernel.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
kernel.o: ../include/stdlib.h ../include/unistd.h ../include/string.h
kernel.o: ../include/memory.h ../include/pagination.h include/gdt.h
kernel.o: include/exception.h ../include/interrupts.h ../include/pci.h
kernel.o: ../include/pci_config.h ../include/pci_types.h include/scheduler.h
kernel.o: ../include/process.h ../include/time.h ../include/keyboard.h
kernel.o: ../include/mouse.h ../include/events.h ../include/floppy.h
kernel.o: include/kpanic.h ../include/kmalloc.h include/vmm.h
kernel.o: ../include/ioports.h include/msr.h ../include/fat.h include/fpu.h
kernel.o: include/vm86.h ../include/beeper.h ../include/debug.h
kernel.o: ../include/shell.h include/ksyscall.h ../include/syscall.h
kernel.o: include/ksem.h ../include/video.h ../include/serial.h
kfcntl.o: ../include/process.h ../include/types.h ../include/libio.h
kfcntl.o: ../include/fcntl.h ../include/pagination.h ../include/stdio.h
kfcntl.o: ../include/kfcntl.h ../include/stdarg.h ../include/fat.h
kfcntl.o: ../include/string.h ../include/stdlib.h ../include/keyboard.h
kfcntl.o: ../include/video.h
kmalloc.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
kmalloc.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
kmalloc.o: ../include/heap.h include/vmm.h ../include/kmalloc.h
kpanic.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
kpanic.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
kpanic.o: include/exception.h include/kpanic.h ../include/process.h
kpanic.o: ../include/pagination.h include/scheduler.h
ksem.o: include/ksem.h ../include/types.h ../include/process.h
ksem.o: ../include/libio.h ../include/fcntl.h ../include/pagination.h
ksem.o: ../include/stdio.h ../include/kfcntl.h ../include/stdarg.h
ksem.o: ../include/kmalloc.h
ksyscall.o: include/msr.h ../include/stdio.h ../include/fcntl.h
ksyscall.o: ../include/types.h ../include/libio.h ../include/kfcntl.h
ksyscall.o: ../include/stdarg.h ../include/string.h ../include/interrupts.h
ksyscall.o: include/ksyscall.h ../include/syscall.h
mbr.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
mbr.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
mbr.o: ../include/floppy.h include/mbr.h
memory.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
memory.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
memory.o: ../include/memory.h
pagination.o: ../include/types.h ../include/memory.h ../include/pagination.h
pagination.o: ../include/stdio.h ../include/fcntl.h ../include/libio.h
pagination.o: ../include/kfcntl.h ../include/stdarg.h
process.o: ../include/types.h ../include/stdlib.h ../include/stdio.h
process.o: ../include/fcntl.h ../include/libio.h ../include/kfcntl.h
process.o: ../include/stdarg.h ../include/string.h ../include/video.h
process.o: include/gdt.h ../include/process.h ../include/pagination.h
process.o: ../include/kmalloc.h ../include/syscall.h ../include/time.h
process.o: ../include/elf.h ../include/debug.h
scheduler.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
scheduler.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
scheduler.o: ../include/process.h ../include/pagination.h include/scheduler.h
scheduler.o: ../include/events.h ../include/time.h ../include/clock.h
scheduler.o: ../include/ioports.h include/gdt.h include/i8259.h
scheduler.o: ../include/syscall.h ../include/debug.h ../include/kmalloc.h
scheduler.o: ../include/string.h
vm86.o: ../include/debug.h include/gdt.h ../include/types.h
vmm.o: ../include/memory.h ../include/types.h ../include/pagination.h
vmm.o: include/vmm.h ../include/stdio.h ../include/fcntl.h ../include/libio.h
vmm.o: ../include/kfcntl.h ../include/stdarg.h
