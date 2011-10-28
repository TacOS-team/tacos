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
clock.o: include/clock.h ../libc/include/time.h ../libc/include/sys/types.h
clock.o: include/ksyscall.h ../include/interrupts.h ../include/heap.h
events.o: ../include/types.h include/i8254.h ../include/interrupts.h
events.o: ../libc/include/time.h ../libc/include/sys/types.h include/clock.h
events.o: include/ksyscall.h ../include/list.h include/events.h
events.o: ../include/debug.h
exception.o: ../include/types.h include/idt.h include/exception.h
fat.o: ../libc/include/ctype.h ../libc/include/fcntl.h ../include/floppy.h
fat.o: ../include/types.h include/kfat.h include/kfcntl.h
fat.o: ../libc/include/libio.h ../libc/include/sys/types.h include/kprocess.h
fat.o: ../include/process.h ../include/vmm.h ../include/signal.h
fat.o: include/ksyscall.h ../libc/include/time.h ../libc/include/sys/stat.h
fat.o: ../include/klog.h ../include/kstdio.h ../include/kmalloc.h
fat.o: ../libc/include/stdio.h ../libc/include/stdarg.h
fat.o: ../libc/include/string.h include/clock.h
fpu.o: include/fpu.h ../include/interrupts.h ../include/types.h
fpu.o: ../include/klog.h ../include/kstdio.h
gdt.o: ../include/types.h include/memory.h include/gdt.h
i8254.o: include/i8254.h ../include/types.h ../include/ioports.h
i8259.o: ../include/types.h ../include/ioports.h
idt.o: ../include/types.h include/idt.h
interrupts.o: ../include/types.h include/idt.h include/i8259.h
interrupts.o: ../include/interrupts.h
kdirent.o: include/kdirent.h include/ksyscall.h ../include/types.h
kdirent.o: ../libc/include/dirent.h ../libc/include/sys/types.h
kdirent.o: ../libc/include/sys/stat.h include/kfat.h include/kfcntl.h
kdirent.o: ../libc/include/libio.h include/kprocess.h ../include/process.h
kdirent.o: ../include/vmm.h ../include/signal.h ../libc/include/time.h
kdirent.o: ../include/kmalloc.h ../libc/include/string.h
kdriver.o: include/kdriver.h ../include/types.h include/kfcntl.h
kdriver.o: ../libc/include/libio.h ../libc/include/sys/types.h
kdriver.o: include/kprocess.h ../include/process.h ../include/vmm.h
kdriver.o: ../include/signal.h include/ksyscall.h ../libc/include/string.h
kernel.o: include/multiboot.h include/idt.h ../include/types.h
kernel.o: include/i8259.h ../libc/include/stdio.h ../libc/include/libio.h
kernel.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
kernel.o: ../libc/include/stdlib.h ../libc/include/malloc.h include/kunistd.h
kernel.o: include/ksyscall.h ../libc/include/string.h include/memory.h
kernel.o: include/pagination.h include/gdt.h include/exception.h
kernel.o: ../include/interrupts.h ../include/pci.h ../include/pci_types.h
kernel.o: ../include/pci_config.h include/scheduler.h ../include/process.h
kernel.o: ../include/vmm.h ../include/signal.h include/clock.h
kernel.o: ../libc/include/time.h ../include/keyboard.h include/kfcntl.h
kernel.o: include/kprocess.h ../include/mouse.h include/events.h
kernel.o: ../include/floppy.h include/kpanic.h ../include/kmalloc.h
kernel.o: ../include/ioports.h include/msr.h include/kfat.h
kernel.o: ../libc/include/sys/stat.h include/fpu.h include/vm86.h
kernel.o: ../include/beeper.h ../include/debug.h ../include/shell.h
kernel.o: ../libc/include/sys/syscall.h include/ksem.h ../include/video.h
kernel.o: ../include/console.h ../include/tty.h include/ksignal.h
kernel.o: include/round_robin.h ../include/rtl8139.h ../include/klog.h
kernel.o: ../include/kstdio.h include/kdriver.h include/kdirent.h
kernel.o: ../libc/include/dirent.h include/drivers/dummy_driver.h
kernel.o: include/drivers/serial.h
kfcntl.o: include/kprocess.h ../include/process.h ../libc/include/libio.h
kfcntl.o: ../libc/include/sys/types.h ../include/types.h ../include/vmm.h
kfcntl.o: ../include/signal.h include/ksyscall.h include/scheduler.h
kfcntl.o: ../libc/include/stdio.h ../libc/include/stdarg.h include/kfat.h
kfcntl.o: include/kfcntl.h ../libc/include/time.h ../libc/include/sys/stat.h
kfcntl.o: ../libc/include/string.h ../include/kmalloc.h include/kdriver.h
kfcntl.o: ../include/keyboard.h ../include/console.h ../include/tty.h
kfcntl.o: ../include/klog.h ../include/kstdio.h ../libc/include/fcntl.h
kmalloc.o: ../include/types.h ../libc/include/malloc.h
kmalloc.o: ../libc/include/sys/types.h include/memory.h ../include/vmm.h
kmalloc.o: ../include/kmalloc.h
kpanic.o: ../include/kstdio.h include/exception.h ../include/types.h
kpanic.o: include/kpanic.h include/kprocess.h ../include/process.h
kpanic.o: ../libc/include/libio.h ../libc/include/sys/types.h
kpanic.o: ../include/vmm.h ../include/signal.h include/ksyscall.h
kpanic.o: include/scheduler.h ../include/symtable.h ../include/console.h
kpanic.o: ../include/tty.h include/kfcntl.h
kprocess.o: ../include/debug.h ../include/elf.h ../libc/include/libio.h
kprocess.o: ../libc/include/sys/types.h include/gdt.h ../include/types.h
kprocess.o: include/kfcntl.h include/kprocess.h ../include/process.h
kprocess.o: ../include/vmm.h ../include/signal.h include/ksyscall.h
kprocess.o: ../include/klog.h ../include/kstdio.h ../include/kmalloc.h
kprocess.o: include/scheduler.h ../libc/include/stdio.h
kprocess.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
kprocess.o: ../libc/include/malloc.h ../libc/include/string.h
kprocess.o: ../include/tty.h ../include/video.h include/pagination.h
ksem.o: include/ksem.h include/ksyscall.h ../include/types.h
ksem.o: include/kprocess.h ../include/process.h ../libc/include/libio.h
ksem.o: ../libc/include/sys/types.h ../include/vmm.h ../include/signal.h
ksem.o: ../libc/include/stdio.h ../libc/include/stdarg.h ../include/kmalloc.h
ksem.o: include/scheduler.h
ksignal.o: include/ksignal.h include/ksyscall.h ../include/types.h
ksignal.o: ../include/signal.h include/kprocess.h ../include/process.h
ksignal.o: ../libc/include/libio.h ../libc/include/sys/types.h
ksignal.o: ../include/vmm.h ../include/kstdio.h include/scheduler.h
ksignal.o: ../include/klog.h ../libc/include/sys/syscall.h
ksignal.o: ../include/interrupts.h
kstdio.o: ../include/video.h ../include/types.h ../libc/include/fcntl.h
kstdio.o: include/kfcntl.h ../libc/include/libio.h
kstdio.o: ../libc/include/sys/types.h include/kprocess.h ../include/process.h
kstdio.o: ../include/vmm.h ../include/signal.h include/ksyscall.h
kstdio.o: ../libc/include/stdlib.h ../libc/include/malloc.h
kstdio.o: ../include/console.h ../include/tty.h
ksyscall.o: include/msr.h ../libc/include/stdio.h ../libc/include/libio.h
ksyscall.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
ksyscall.o: ../libc/include/string.h ../include/interrupts.h
ksyscall.o: ../include/types.h include/ksyscall.h ../include/klog.h
ksyscall.o: ../include/kstdio.h
kunistd.o: include/kprocess.h ../include/process.h ../libc/include/libio.h
kunistd.o: ../libc/include/sys/types.h ../include/types.h ../include/vmm.h
kunistd.o: ../include/signal.h include/ksyscall.h include/scheduler.h
kunistd.o: include/kfcntl.h ../include/klog.h ../include/kstdio.h
kunistd.o: ../libc/include/stdio.h ../libc/include/stdarg.h
mbr.o: ../include/floppy.h ../include/types.h include/mbr.h
memory.o: ../include/types.h include/memory.h
module.o: ../include/klog.h ../include/kstdio.h include/module.h
module.o: ../include/types.h ../include/kmalloc.h ../include/symtable.h
module.o: ../libc/include/stdio.h ../libc/include/libio.h
module.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
module.o: ../include/elf.h ../libc/include/string.h ../libc/include/stdlib.h
module.o: ../libc/include/malloc.h
pagination.o: ../include/types.h include/memory.h include/pagination.h
pagination.o: ../include/vmm.h
round_robin.o: ../include/kmalloc.h ../include/types.h include/kprocess.h
round_robin.o: ../include/process.h ../libc/include/libio.h
round_robin.o: ../libc/include/sys/types.h ../include/vmm.h
round_robin.o: ../include/signal.h include/ksyscall.h include/scheduler.h
scheduler.o: include/clock.h ../include/types.h ../libc/include/time.h
scheduler.o: ../libc/include/sys/types.h include/ksyscall.h include/context.h
scheduler.o: ../include/process.h ../libc/include/libio.h ../include/vmm.h
scheduler.o: ../include/signal.h ../include/debug.h include/events.h
scheduler.o: include/gdt.h include/i8259.h ../include/kmalloc.h
scheduler.o: include/kprocess.h include/ksignal.h ../include/interrupts.h
scheduler.o: ../include/ioports.h include/scheduler.h include/pagination.h
scheduler.o: ../libc/include/string.h ../libc/include/unistd.h
scheduler.o: ../libc/include/sys/syscall.h
symtable.o: ../libc/include/string.h ../libc/include/sys/types.h
symtable.o: ../libc/include/stdio.h ../libc/include/libio.h
symtable.o: ../libc/include/stdarg.h ../include/kmalloc.h ../include/types.h
symtable.o: ../include/elf.h ../include/symtable.h ../include/klog.h
symtable.o: ../include/kstdio.h
vm86.o: ../include/debug.h include/gdt.h ../include/types.h
vmm.o: include/memory.h ../include/types.h include/pagination.h
vmm.o: ../include/vmm.h ../include/process.h ../libc/include/libio.h
vmm.o: ../libc/include/sys/types.h ../include/signal.h include/ksyscall.h
vmm.o: ../include/kstdio.h
