INCLUDES=-I../include/ -Iinclude/ -I../libc/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: boot.o exception_wrappers.o interrupts_wrappers.o kstdio.o kernel.o i8259.o idt.o gdt.o memory.o exception.o interrupts.o scheduler.o kprocess.o pagination.o kpanic.o ksyscall.o vmm.o kmalloc.o fat.o fpu.o vm86.o ksem.o kfcntl.o kunistd.o ksignal.o

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

exception.o: ../include/types.h include/idt.h include/exception.h
fat.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
fat.o: ../include/types.h ../libc/include/libio.h ../libc/include/stdarg.h
fat.o: ../include/floppy.h ../libc/include/string.h ../include/fat.h
fat.o: ../include/debug.h
fpu.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
fpu.o: ../include/types.h ../libc/include/libio.h ../libc/include/stdarg.h
fpu.o: include/fpu.h ../include/interrupts.h
gdt.o: ../include/types.h ../include/memory.h include/gdt.h
i8259.o: ../include/types.h ../include/ioports.h
idt.o: ../include/types.h include/idt.h
interrupts.o: ../include/types.h include/idt.h include/i8259.h
interrupts.o: ../include/interrupts.h
kernel.o: include/multiboot.h include/idt.h ../include/types.h
kernel.o: include/i8259.h ../libc/include/stdio.h ../libc/include/fcntl.h
kernel.o: ../include/kfcntl.h ../libc/include/libio.h
kernel.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
kernel.o: ../libc/include/malloc.h ../include/kunistd.h
kernel.o: ../libc/include/string.h ../include/memory.h
kernel.o: ../include/pagination.h include/gdt.h include/exception.h
kernel.o: ../include/interrupts.h ../include/pci.h ../include/pci_config.h
kernel.o: ../include/pci_types.h include/scheduler.h ../include/process.h
kernel.o: ../include/vmm.h ../include/signal.h ../libc/include/time.h
kernel.o: ../include/keyboard.h ../include/mouse.h ../include/events.h
kernel.o: ../include/floppy.h include/kpanic.h include/kprocess.h
kernel.o: ../include/kmalloc.h ../include/ioports.h include/msr.h
kernel.o: ../include/fat.h include/fpu.h include/vm86.h ../include/beeper.h
kernel.o: ../include/debug.h ../include/shell.h include/ksyscall.h
kernel.o: ../include/syscall.h include/ksem.h ../include/video.h
kernel.o: ../include/serial.h ../include/console.h include/ksignal.h
kfcntl.o: include/kprocess.h ../include/process.h ../libc/include/libio.h
kfcntl.o: ../libc/include/fcntl.h ../include/types.h ../include/pagination.h
kfcntl.o: ../include/vmm.h ../include/signal.h ../libc/include/stdio.h
kfcntl.o: ../include/kfcntl.h ../libc/include/stdarg.h ../include/fat.h
kfcntl.o: ../libc/include/string.h ../include/kmalloc.h ../include/keyboard.h
kfcntl.o: ../include/console.h ../include/tty.h
kmalloc.o: ../include/types.h ../libc/include/malloc.h ../include/memory.h
kmalloc.o: ../include/vmm.h ../include/kmalloc.h
kpanic.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
kpanic.o: ../include/types.h ../libc/include/libio.h ../libc/include/stdarg.h
kpanic.o: include/exception.h include/kpanic.h include/kprocess.h
kpanic.o: ../include/process.h ../include/pagination.h ../include/vmm.h
kpanic.o: ../include/signal.h include/scheduler.h
kprocess.o: ../include/types.h ../libc/include/stdlib.h
kprocess.o: ../libc/include/malloc.h ../libc/include/stdio.h
kprocess.o: ../libc/include/fcntl.h ../include/kfcntl.h
kprocess.o: ../libc/include/libio.h ../libc/include/stdarg.h
kprocess.o: ../libc/include/string.h ../include/video.h include/gdt.h
kprocess.o: include/kprocess.h ../include/process.h ../include/pagination.h
kprocess.o: ../include/vmm.h ../include/signal.h ../include/kmalloc.h
kprocess.o: ../include/syscall.h ../include/elf.h ../include/debug.h
ksem.o: include/ksem.h ../include/types.h include/kprocess.h
ksem.o: ../include/process.h ../libc/include/libio.h ../libc/include/fcntl.h
ksem.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
ksem.o: ../libc/include/stdio.h ../include/kfcntl.h ../libc/include/stdarg.h
ksem.o: ../include/kmalloc.h
ksignal.o: include/ksignal.h ../include/signal.h ../include/types.h
ksignal.o: include/kprocess.h ../include/process.h ../libc/include/libio.h
ksignal.o: ../libc/include/fcntl.h ../include/pagination.h ../include/vmm.h
kstdio.o: ../include/video.h ../include/types.h ../libc/include/fcntl.h
kstdio.o: ../include/kfcntl.h ../libc/include/libio.h
kstdio.o: ../libc/include/stdlib.h ../libc/include/malloc.h
kstdio.o: ../include/console.h
ksyscall.o: include/msr.h ../libc/include/stdio.h ../libc/include/fcntl.h
ksyscall.o: ../include/kfcntl.h ../include/types.h ../libc/include/libio.h
ksyscall.o: ../libc/include/stdarg.h ../libc/include/string.h
ksyscall.o: ../include/interrupts.h include/ksyscall.h ../include/syscall.h
kunistd.o: include/kprocess.h ../include/process.h ../libc/include/libio.h
kunistd.o: ../libc/include/fcntl.h ../include/types.h ../include/pagination.h
kunistd.o: ../include/vmm.h ../include/signal.h ../include/kfcntl.h
mbr.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
mbr.o: ../include/types.h ../libc/include/libio.h ../libc/include/stdarg.h
mbr.o: ../include/floppy.h include/mbr.h
memory.o: ../include/types.h ../libc/include/stdio.h ../libc/include/fcntl.h
memory.o: ../include/kfcntl.h ../libc/include/libio.h
memory.o: ../libc/include/stdarg.h ../include/memory.h
pagination.o: ../include/types.h ../include/memory.h ../include/pagination.h
pagination.o: ../include/vmm.h ../libc/include/stdio.h
pagination.o: ../libc/include/fcntl.h ../include/kfcntl.h
pagination.o: ../libc/include/libio.h ../libc/include/stdarg.h
scheduler.o: ../libc/include/stdio.h ../libc/include/fcntl.h
scheduler.o: ../include/kfcntl.h ../include/types.h ../libc/include/libio.h
scheduler.o: ../libc/include/stdarg.h include/kprocess.h ../include/process.h
scheduler.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
scheduler.o: include/scheduler.h ../include/events.h ../libc/include/time.h
scheduler.o: ../include/clock.h ../include/ioports.h include/gdt.h
scheduler.o: include/i8259.h ../include/syscall.h ../include/debug.h
scheduler.o: ../include/kmalloc.h ../libc/include/string.h
vm86.o: ../include/debug.h include/gdt.h ../include/types.h
vmm.o: ../include/memory.h ../include/types.h ../include/pagination.h
vmm.o: ../include/vmm.h ../include/process.h ../libc/include/libio.h
vmm.o: ../libc/include/fcntl.h ../include/signal.h ../libc/include/stdio.h
vmm.o: ../include/kfcntl.h ../libc/include/stdarg.h include/ksyscall.h
vmm.o: ../include/syscall.h
