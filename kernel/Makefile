INCLUDES=-I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: boot.o exception_wrappers.o interrupts_wrappers.o kernel.o i8259.o idt.o gdt.o memory.o exception.o interrupts.o scheduler.o kprocess.o pagination.o kpanic.o ksyscall.o vmm.o kmalloc.o fat.o fpu.o vm86.o ksem.o elf.o kfcntl.o kunistd.o video.o stdfiles.o

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
kernel.o: ../include/stdlib.h ../include/kunistd.h ../include/string.h
kernel.o: ../include/memory.h ../include/pagination.h include/gdt.h
kernel.o: include/exception.h ../include/interrupts.h ../include/pci.h
kernel.o: ../include/pci_config.h ../include/pci_types.h include/scheduler.h
kernel.o: ../include/process.h ../include/vmm.h ../include/time.h
kernel.o: ../include/keyboard.h ../include/mouse.h ../include/events.h
kernel.o: ../include/floppy.h include/kpanic.h include/kprocess.h
kernel.o: ../include/kmalloc.h ../include/ioports.h include/msr.h
kernel.o: ../include/fat.h include/fpu.h include/vm86.h ../include/beeper.h
kernel.o: ../include/debug.h ../include/shell.h include/ksyscall.h
kernel.o: ../include/syscall.h include/ksem.h include/video.h
kernel.o: ../include/serial.h
kfcntl.o: include/kprocess.h ../include/process.h ../include/types.h
kfcntl.o: ../include/libio.h ../include/fcntl.h ../include/pagination.h
kfcntl.o: ../include/vmm.h ../include/stdio.h ../include/kfcntl.h
kfcntl.o: ../include/stdarg.h ../include/fat.h ../include/string.h
kfcntl.o: ../include/kmalloc.h ../include/keyboard.h include/video.h
kmalloc.o: ../include/types.h ../include/malloc.h ../include/memory.h
kmalloc.o: ../include/vmm.h ../include/kmalloc.h
kpanic.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
kpanic.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
kpanic.o: include/exception.h include/kpanic.h include/kprocess.h
kpanic.o: ../include/process.h ../include/pagination.h ../include/vmm.h
kpanic.o: include/scheduler.h
kprocess.o: ../include/types.h ../include/stdlib.h ../include/stdio.h
kprocess.o: ../include/fcntl.h ../include/libio.h ../include/kfcntl.h
kprocess.o: ../include/stdarg.h ../include/string.h include/video.h
kprocess.o: include/gdt.h include/kprocess.h ../include/process.h
kprocess.o: ../include/pagination.h ../include/vmm.h ../include/kmalloc.h
kprocess.o: ../include/syscall.h ../include/time.h ../include/elf.h
kprocess.o: ../include/debug.h
ksem.o: include/ksem.h ../include/types.h include/kprocess.h
ksem.o: ../include/process.h ../include/libio.h ../include/fcntl.h
ksem.o: ../include/pagination.h ../include/vmm.h ../include/stdio.h
ksem.o: ../include/kfcntl.h ../include/stdarg.h ../include/kmalloc.h
ksyscall.o: include/msr.h ../include/stdio.h ../include/fcntl.h
ksyscall.o: ../include/types.h ../include/libio.h ../include/kfcntl.h
ksyscall.o: ../include/stdarg.h ../include/string.h ../include/interrupts.h
ksyscall.o: include/ksyscall.h ../include/syscall.h
kunistd.o: include/kprocess.h ../include/process.h ../include/types.h
kunistd.o: ../include/libio.h ../include/fcntl.h ../include/pagination.h
kunistd.o: ../include/vmm.h ../include/kfcntl.h
mbr.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
mbr.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
mbr.o: ../include/floppy.h include/mbr.h
memory.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
memory.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
memory.o: ../include/memory.h
pagination.o: ../include/types.h ../include/memory.h ../include/pagination.h
pagination.o: ../include/vmm.h ../include/stdio.h ../include/fcntl.h
pagination.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
scheduler.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
scheduler.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
scheduler.o: include/kprocess.h ../include/process.h ../include/pagination.h
scheduler.o: ../include/vmm.h include/scheduler.h ../include/events.h
scheduler.o: ../include/time.h ../include/clock.h ../include/ioports.h
scheduler.o: include/gdt.h include/i8259.h ../include/syscall.h
scheduler.o: ../include/debug.h ../include/kmalloc.h ../include/string.h
stdfiles.o: ../include/types.h ../include/libio.h ../include/fcntl.h
stdfiles.o: ../include/string.h ../include/kmalloc.h
video.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
video.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
video.o: ../include/ioports.h ../include/ctype.h include/video.h
video.o: ../include/stdlib.h ../include/syscall.h include/kprocess.h
video.o: ../include/process.h ../include/pagination.h ../include/vmm.h
vm86.o: ../include/debug.h include/gdt.h ../include/types.h
vmm.o: ../include/memory.h ../include/types.h ../include/pagination.h
vmm.o: ../include/vmm.h ../include/process.h ../include/libio.h
vmm.o: ../include/fcntl.h ../include/stdio.h ../include/kfcntl.h
vmm.o: ../include/stdarg.h include/ksyscall.h ../include/syscall.h
