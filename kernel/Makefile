INCLUDES=-I../include/ -Iinclude/ -I../libc/include/
CFLAGS=-W -Wall -g -ffreestanding -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -finline-functions -I`pwd` -m32

SRCS = *.c

all: boot.o exception_wrappers.o interrupts_wrappers.o kdirent.o kstdio.o kernel.o i8259.o idt.o gdt.o memory.o exception.o interrupts.o scheduler.o kprocess.o pagination.o kpanic.o ksyscall.o vmm.o kmalloc.o fat.o fpu.o vm86.o ksem.o kfcntl.o kunistd.o ksignal.o round_robin.o events.o clock.o i8254.o kdriver.o drivers/dummy_driver.o drivers/serial.o symtable.o module.o

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

clock.o: ../include/types.h ../include/ioports.h include/i8254.h
clock.o: include/clock.h ../libc/include/time.h ../include/interrupts.h
clock.o: ../include/heap.h
events.o: ../include/types.h include/i8254.h ../include/interrupts.h
events.o: ../libc/include/time.h include/clock.h ../include/list.h
events.o: include/events.h ../libc/include/stdio.h ../libc/include/fcntl.h
events.o: ../libc/include/libio.h ../libc/include/stdarg.h ../include/debug.h
exception.o: ../include/types.h include/idt.h include/exception.h
fat.o: ../libc/include/ctype.h ../include/errno.h ../libc/include/fcntl.h
fat.o: ../include/floppy.h ../include/types.h include/kfat.h include/kfcntl.h
fat.o: ../libc/include/libio.h include/kprocess.h ../include/process.h
fat.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
fat.o: include/ksyscall.h ../libc/include/time.h ../include/klog.h
fat.o: ../include/kmalloc.h ../libc/include/stdio.h ../libc/include/stdarg.h
fat.o: ../libc/include/string.h include/clock.h
fpu.o: ../libc/include/stdio.h ../libc/include/fcntl.h
fpu.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
fpu.o: include/fpu.h ../include/interrupts.h ../include/klog.h
gdt.o: ../include/types.h ../include/memory.h include/gdt.h
i8254.o: include/i8254.h ../include/types.h ../include/ioports.h
i8259.o: ../include/types.h ../include/ioports.h
idt.o: ../include/types.h include/idt.h
interrupts.o: ../include/types.h include/idt.h include/i8259.h
interrupts.o: ../include/interrupts.h
kdirent.o: include/kdirent.h include/ksyscall.h ../include/types.h
kdirent.o: ../libc/include/dirent.h include/kfat.h include/kfcntl.h
kdirent.o: ../libc/include/libio.h ../libc/include/fcntl.h include/kprocess.h
kdirent.o: ../include/process.h ../include/pagination.h ../include/vmm.h
kdirent.o: ../include/signal.h ../libc/include/time.h ../include/kmalloc.h
kdirent.o: ../libc/include/string.h
kdriver.o: include/kdriver.h ../include/types.h include/kfcntl.h
kdriver.o: ../libc/include/libio.h ../libc/include/fcntl.h include/kprocess.h
kdriver.o: ../include/process.h ../include/pagination.h ../include/vmm.h
kdriver.o: ../include/signal.h include/ksyscall.h ../libc/include/string.h
kernel.o: include/multiboot.h include/idt.h ../include/types.h
kernel.o: include/i8259.h ../libc/include/stdio.h ../libc/include/fcntl.h
kernel.o: ../libc/include/libio.h ../libc/include/stdarg.h
kernel.o: ../libc/include/stdlib.h ../libc/include/malloc.h include/kunistd.h
kernel.o: include/ksyscall.h ../libc/include/string.h ../include/memory.h
kernel.o: ../include/pagination.h include/gdt.h include/exception.h
kernel.o: ../include/interrupts.h ../include/pci.h ../include/pci_types.h
kernel.o: ../include/pci_config.h include/scheduler.h ../include/process.h
kernel.o: ../include/vmm.h ../include/signal.h ../libc/include/time.h
kernel.o: ../include/keyboard.h include/kfcntl.h include/kprocess.h
kernel.o: ../include/mouse.h include/events.h ../include/floppy.h
kernel.o: include/kpanic.h ../include/kmalloc.h ../include/ioports.h
kernel.o: include/msr.h include/kfat.h include/fpu.h include/vm86.h
kernel.o: ../include/beeper.h ../include/debug.h ../include/shell.h
kernel.o: ../include/syscall.h include/ksem.h ../include/video.h
kernel.o: ../include/console.h ../include/tty.h include/ksignal.h
kernel.o: include/round_robin.h ../include/rtl8139.h ../include/klog.h
kernel.o: include/kdriver.h include/kdirent.h ../libc/include/dirent.h
kernel.o: include/drivers/dummy_driver.h include/drivers/serial.h
kfcntl.o: include/kprocess.h ../include/process.h ../libc/include/libio.h
kfcntl.o: ../libc/include/fcntl.h ../include/types.h ../include/pagination.h
kfcntl.o: ../include/vmm.h ../include/signal.h include/ksyscall.h
kfcntl.o: include/scheduler.h ../libc/include/stdio.h
kfcntl.o: ../libc/include/stdarg.h include/kfat.h include/kfcntl.h
kfcntl.o: ../libc/include/time.h ../libc/include/string.h
kfcntl.o: ../include/kmalloc.h include/kdriver.h ../include/keyboard.h
kfcntl.o: ../include/console.h ../include/tty.h ../include/klog.h
kmalloc.o: ../include/types.h ../libc/include/malloc.h ../include/memory.h
kmalloc.o: ../include/vmm.h ../include/kmalloc.h
kpanic.o: ../libc/include/stdio.h ../libc/include/fcntl.h
kpanic.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
kpanic.o: include/exception.h include/kpanic.h include/kprocess.h
kpanic.o: ../include/process.h ../include/pagination.h ../include/vmm.h
kpanic.o: ../include/signal.h include/ksyscall.h include/scheduler.h
kpanic.o: ../include/symtable.h ../include/console.h ../include/tty.h
kpanic.o: include/kfcntl.h
kprocess.o: ../include/debug.h ../include/elf.h ../libc/include/libio.h
kprocess.o: ../libc/include/fcntl.h ../include/types.h include/gdt.h
kprocess.o: include/kfcntl.h include/kprocess.h ../include/process.h
kprocess.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
kprocess.o: include/ksyscall.h ../include/klog.h ../include/kmalloc.h
kprocess.o: include/scheduler.h ../libc/include/stdio.h
kprocess.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
kprocess.o: ../libc/include/malloc.h ../libc/include/string.h
kprocess.o: ../include/tty.h ../include/video.h
ksem.o: include/ksem.h include/ksyscall.h ../include/types.h
ksem.o: include/kprocess.h ../include/process.h ../libc/include/libio.h
ksem.o: ../libc/include/fcntl.h ../include/pagination.h ../include/vmm.h
ksem.o: ../include/signal.h ../libc/include/stdio.h ../libc/include/stdarg.h
ksem.o: ../include/kmalloc.h include/scheduler.h
ksignal.o: include/ksignal.h include/ksyscall.h ../include/types.h
ksignal.o: ../include/signal.h include/kprocess.h ../include/process.h
ksignal.o: ../libc/include/libio.h ../libc/include/fcntl.h
ksignal.o: ../include/pagination.h ../include/vmm.h include/kstdio.h
ksignal.o: include/scheduler.h ../include/klog.h
kstdio.o: ../include/video.h ../include/types.h ../libc/include/fcntl.h
kstdio.o: include/kfcntl.h ../libc/include/libio.h include/kprocess.h
kstdio.o: ../include/process.h ../include/pagination.h ../include/vmm.h
kstdio.o: ../include/signal.h include/ksyscall.h ../libc/include/stdlib.h
kstdio.o: ../libc/include/malloc.h ../include/console.h ../include/tty.h
ksyscall.o: include/msr.h ../libc/include/stdio.h ../libc/include/fcntl.h
ksyscall.o: ../libc/include/libio.h ../include/types.h
ksyscall.o: ../libc/include/stdarg.h ../libc/include/string.h
ksyscall.o: ../include/interrupts.h include/ksyscall.h ../include/syscall.h
ksyscall.o: ../include/klog.h
kunistd.o: include/kprocess.h ../include/process.h ../libc/include/libio.h
kunistd.o: ../libc/include/fcntl.h ../include/types.h ../include/pagination.h
kunistd.o: ../include/vmm.h ../include/signal.h include/ksyscall.h
kunistd.o: include/scheduler.h include/kfcntl.h ../include/klog.h
kunistd.o: ../libc/include/stdio.h ../libc/include/stdarg.h
mbr.o: ../libc/include/stdio.h ../libc/include/fcntl.h
mbr.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
mbr.o: ../include/floppy.h include/mbr.h
memory.o: ../include/types.h ../libc/include/stdio.h ../libc/include/fcntl.h
memory.o: ../libc/include/libio.h ../libc/include/stdarg.h
memory.o: ../include/memory.h
module.o: ../include/klog.h include/module.h ../include/types.h
module.o: ../include/kmalloc.h ../include/symtable.h ../include/elf.h
module.o: ../libc/include/libio.h ../libc/include/fcntl.h
module.o: ../libc/include/stdio.h ../libc/include/stdarg.h
module.o: ../libc/include/string.h ../libc/include/stdlib.h
module.o: ../libc/include/malloc.h
pagination.o: ../include/types.h ../include/memory.h ../include/pagination.h
pagination.o: ../include/vmm.h ../libc/include/stdio.h
pagination.o: ../libc/include/fcntl.h ../libc/include/libio.h
pagination.o: ../libc/include/stdarg.h
round_robin.o: ../include/kmalloc.h ../include/types.h include/kprocess.h
round_robin.o: ../include/process.h ../libc/include/libio.h
round_robin.o: ../libc/include/fcntl.h ../include/pagination.h
round_robin.o: ../include/vmm.h ../include/signal.h include/ksyscall.h
round_robin.o: include/scheduler.h
scheduler.o: ../libc/include/stdio.h ../libc/include/fcntl.h
scheduler.o: ../libc/include/libio.h ../include/types.h
scheduler.o: ../libc/include/stdarg.h include/kprocess.h ../include/process.h
scheduler.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
scheduler.o: include/ksyscall.h include/scheduler.h include/events.h
scheduler.o: ../libc/include/time.h include/clock.h ../include/ioports.h
scheduler.o: include/gdt.h include/i8259.h ../include/syscall.h
scheduler.o: ../include/debug.h ../include/kmalloc.h ../libc/include/string.h
scheduler.o: include/ksignal.h
symtable.o: ../libc/include/string.h ../include/types.h
symtable.o: ../libc/include/stdio.h ../libc/include/fcntl.h
symtable.o: ../libc/include/libio.h ../libc/include/stdarg.h
symtable.o: ../include/kmalloc.h ../include/elf.h ../include/symtable.h
symtable.o: ../include/klog.h
vm86.o: ../include/debug.h include/gdt.h ../include/types.h
vmm.o: ../include/memory.h ../include/types.h ../include/pagination.h
vmm.o: ../include/vmm.h ../include/process.h ../libc/include/libio.h
vmm.o: ../libc/include/fcntl.h ../include/signal.h ../libc/include/stdio.h
vmm.o: ../libc/include/stdarg.h include/ksyscall.h ../include/syscall.h
