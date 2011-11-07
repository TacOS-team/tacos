INCLUDES=-I../include/ -Iinclude/ -I../libc/include/
CFLAGS=-W -Wall $(WE) -g -ffreestanding -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -finline-functions -I`pwd` -m32

SRCS = *.c

all: boot.o exception_wrappers.o interrupts_wrappers.o kdirent.o kstdio.o kernel.o i8259.o idt.o gdt.o memory.o exception.o interrupts.o scheduler.o kprocess.o pagination.o kpanic.o ksyscall.o vmm.o kmalloc.o fat.o fpu.o vm86.o ksem.o kfcntl.o kunistd.o ksignal.o round_robin.o events.o clock.o i8254.o kdriver.o drivers/dummy_driver.o drivers/serial.o drivers/mouse.o symtable.o module.o init.o

boot.o: boot.S
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) -fno-stack-protector

exception_wrappers.o: exception_wrappers.S include/exception.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) -DASM_SOURCE=1 -fno-stack-protector

interrupts_wrappers.o: interrupts_wrappers.S ../include/interrupts.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) -DASM_SOURCE=1 -fno-stack-protector

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o drivers/*.o

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
fat.o: include/ksyscall.h ../include/process.h ../include/elf.h
fat.o: ../libc/include/signal.h ../include/symtable.h ../include/vmm.h
fat.o: ../libc/include/time.h ../libc/include/sys/stat.h ../include/klog.h
fat.o: ../include/kstdio.h ../include/kmalloc.h ../libc/include/string.h
fat.o: include/clock.h
fpu.o: include/fpu.h ../include/interrupts.h ../include/types.h
fpu.o: ../include/klog.h ../include/kstdio.h
gdt.o: ../include/types.h include/memory.h include/gdt.h
i8254.o: include/i8254.h ../include/types.h ../include/ioports.h
i8259.o: ../include/types.h ../include/ioports.h
idt.o: ../include/types.h include/idt.h
init.o: include/kdriver.h ../include/types.h include/kfcntl.h
init.o: ../libc/include/libio.h ../libc/include/sys/types.h
init.o: include/kprocess.h include/ksyscall.h ../include/process.h
init.o: ../include/elf.h ../libc/include/signal.h ../include/symtable.h
init.o: ../include/vmm.h ../include/klog.h ../include/kstdio.h
init.o: include/scheduler.h include/drivers/dummy_driver.h
init.o: include/drivers/mouse.h ../libc/include/stdio.h
init.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
init.o: ../libc/include/malloc.h
interrupts.o: ../include/types.h include/idt.h include/i8259.h
interrupts.o: ../include/interrupts.h
kdirent.o: include/kdirent.h include/ksyscall.h ../include/types.h
kdirent.o: ../libc/include/dirent.h ../libc/include/sys/types.h
kdirent.o: ../libc/include/sys/stat.h include/kfat.h include/kfcntl.h
kdirent.o: ../libc/include/libio.h include/kprocess.h ../include/process.h
kdirent.o: ../include/elf.h ../libc/include/signal.h ../include/symtable.h
kdirent.o: ../include/vmm.h ../libc/include/time.h ../include/kmalloc.h
kdirent.o: ../libc/include/string.h
kdriver.o: include/kdriver.h ../include/types.h include/kfcntl.h
kdriver.o: ../libc/include/libio.h ../libc/include/sys/types.h
kdriver.o: include/kprocess.h include/ksyscall.h ../include/process.h
kdriver.o: ../include/elf.h ../libc/include/signal.h ../include/symtable.h
kdriver.o: ../include/vmm.h ../libc/include/string.h
kernel.o: include/multiboot.h include/idt.h ../include/types.h
kernel.o: include/i8259.h include/kunistd.h include/ksyscall.h
kernel.o: ../libc/include/string.h ../libc/include/sys/types.h
kernel.o: include/memory.h include/pagination.h include/gdt.h
kernel.o: include/exception.h ../include/interrupts.h ../include/pci.h
kernel.o: ../include/pci_types.h ../include/pci_config.h include/scheduler.h
kernel.o: include/kprocess.h ../libc/include/libio.h ../include/process.h
kernel.o: ../include/elf.h ../libc/include/signal.h ../include/symtable.h
kernel.o: ../include/vmm.h include/clock.h ../libc/include/time.h
kernel.o: ../include/keyboard.h include/kfcntl.h include/events.h
kernel.o: ../include/floppy.h include/kpanic.h ../include/kmalloc.h
kernel.o: ../include/ioports.h include/msr.h include/kfat.h
kernel.o: ../libc/include/sys/stat.h include/fpu.h include/vm86.h
kernel.o: ../include/beeper.h ../include/debug.h ../include/shell.h
kernel.o: ../libc/include/sys/syscall.h include/ksem.h ../include/video.h
kernel.o: ../include/console.h ../include/tty.h include/ksignal.h
kernel.o: include/round_robin.h ../include/rtl8139.h ../include/klog.h
kernel.o: ../include/kstdio.h include/kdriver.h include/kdirent.h
kernel.o: ../libc/include/dirent.h ../include/vga.h ../include/vga_types.h
kernel.o: include/init.h include/drivers/dummy_driver.h
kernel.o: include/drivers/serial.h include/drivers/mouse.h
kfcntl.o: include/kprocess.h include/ksyscall.h ../include/types.h
kfcntl.o: ../libc/include/libio.h ../libc/include/sys/types.h
kfcntl.o: ../include/process.h ../include/elf.h ../libc/include/signal.h
kfcntl.o: ../include/symtable.h ../include/vmm.h include/scheduler.h
kfcntl.o: include/kfat.h include/kfcntl.h ../libc/include/time.h
kfcntl.o: ../libc/include/sys/stat.h ../libc/include/string.h
kfcntl.o: ../include/kmalloc.h include/kdriver.h ../include/keyboard.h
kfcntl.o: ../include/console.h ../include/tty.h ../include/klog.h
kfcntl.o: ../include/kstdio.h ../libc/include/fcntl.h
kmalloc.o: ../include/types.h include/memory.h ../include/vmm.h
kmalloc.o: ../include/kmalloc.h
kpanic.o: ../include/kstdio.h include/exception.h ../include/types.h
kpanic.o: ../include/interrupts.h include/kpanic.h include/kprocess.h
kpanic.o: include/ksyscall.h ../libc/include/libio.h
kpanic.o: ../libc/include/sys/types.h ../include/process.h ../include/elf.h
kpanic.o: ../libc/include/signal.h ../include/symtable.h ../include/vmm.h
kpanic.o: include/scheduler.h ../include/console.h ../include/tty.h
kpanic.o: include/kfcntl.h
kprocess.o: ../include/debug.h ../include/elf.h ../include/types.h
kprocess.o: include/gdt.h include/kfcntl.h ../libc/include/libio.h
kprocess.o: ../libc/include/sys/types.h include/kprocess.h include/ksyscall.h
kprocess.o: ../include/process.h ../libc/include/signal.h
kprocess.o: ../include/symtable.h ../include/vmm.h ../include/klog.h
kprocess.o: ../include/kstdio.h ../include/kmalloc.h include/scheduler.h
kprocess.o: ../libc/include/string.h ../include/tty.h include/pagination.h
ksem.o: include/ksem.h include/ksyscall.h ../include/types.h
ksem.o: include/kprocess.h ../libc/include/libio.h
ksem.o: ../libc/include/sys/types.h ../include/process.h ../include/elf.h
ksem.o: ../libc/include/signal.h ../include/symtable.h ../include/vmm.h
ksem.o: ../include/kmalloc.h include/scheduler.h
ksignal.o: include/gdt.h ../include/types.h include/ksignal.h
ksignal.o: include/ksyscall.h ../libc/include/signal.h
ksignal.o: ../libc/include/sys/types.h include/kprocess.h
ksignal.o: ../libc/include/libio.h ../include/process.h ../include/elf.h
ksignal.o: ../include/symtable.h ../include/vmm.h ../include/kstdio.h
ksignal.o: include/scheduler.h ../include/klog.h
ksignal.o: ../libc/include/sys/syscall.h ../include/interrupts.h
kstdio.o: ../libc/include/fcntl.h include/kfcntl.h ../include/types.h
kstdio.o: ../libc/include/libio.h ../libc/include/sys/types.h
kstdio.o: include/kprocess.h include/ksyscall.h ../include/process.h
kstdio.o: ../include/elf.h ../libc/include/signal.h ../include/symtable.h
kstdio.o: ../include/vmm.h ../include/console.h ../include/tty.h
ksyscall.o: include/msr.h ../libc/include/string.h
ksyscall.o: ../libc/include/sys/types.h ../include/interrupts.h
ksyscall.o: ../include/types.h include/ksyscall.h ../include/klog.h
ksyscall.o: ../include/kstdio.h
kunistd.o: include/kprocess.h include/ksyscall.h ../include/types.h
kunistd.o: ../libc/include/libio.h ../libc/include/sys/types.h
kunistd.o: ../include/process.h ../include/elf.h ../libc/include/signal.h
kunistd.o: ../include/symtable.h ../include/vmm.h include/scheduler.h
kunistd.o: include/kfcntl.h ../include/klog.h ../include/kstdio.h
mbr.o: ../include/floppy.h ../include/types.h include/mbr.h
memory.o: ../include/types.h include/memory.h
module.o: ../include/klog.h ../include/kstdio.h include/module.h
module.o: ../include/types.h ../include/kmalloc.h ../include/symtable.h
module.o: ../include/elf.h ../libc/include/stdio.h ../libc/include/libio.h
module.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
module.o: ../libc/include/unistd.h ../libc/include/fcntl.h
module.o: ../libc/include/string.h ../libc/include/stdlib.h
module.o: ../libc/include/malloc.h
pagination.o: ../include/types.h include/memory.h include/pagination.h
pagination.o: ../include/vmm.h
round_robin.o: ../include/kmalloc.h ../include/types.h include/kprocess.h
round_robin.o: include/ksyscall.h ../libc/include/libio.h
round_robin.o: ../libc/include/sys/types.h ../include/process.h
round_robin.o: ../include/elf.h ../libc/include/signal.h
round_robin.o: ../include/symtable.h ../include/vmm.h include/scheduler.h
scheduler.o: include/clock.h ../include/types.h ../libc/include/time.h
scheduler.o: ../libc/include/sys/types.h include/ksyscall.h include/context.h
scheduler.o: include/kprocess.h ../libc/include/libio.h ../include/process.h
scheduler.o: ../include/elf.h ../libc/include/signal.h ../include/symtable.h
scheduler.o: ../include/vmm.h ../include/debug.h include/events.h
scheduler.o: include/gdt.h include/i8259.h ../include/klog.h
scheduler.o: ../include/kstdio.h ../include/kmalloc.h include/ksignal.h
scheduler.o: ../include/interrupts.h ../include/ioports.h include/scheduler.h
scheduler.o: include/pagination.h ../libc/include/string.h
scheduler.o: ../libc/include/unistd.h ../libc/include/sys/syscall.h
symtable.o: ../libc/include/string.h ../libc/include/sys/types.h
symtable.o: include/kprocess.h include/ksyscall.h ../include/types.h
symtable.o: ../libc/include/libio.h ../include/process.h ../include/elf.h
symtable.o: ../libc/include/signal.h ../include/symtable.h ../include/vmm.h
symtable.o: ../include/kmalloc.h ../include/klog.h ../include/kstdio.h
symtable.o: ../libc/include/fcntl.h
vm86.o: ../include/debug.h include/gdt.h ../include/types.h
vmm.o: include/memory.h ../include/types.h include/pagination.h
vmm.o: ../include/vmm.h ../include/process.h ../include/elf.h
vmm.o: include/ksyscall.h ../include/kstdio.h include/scheduler.h
vmm.o: include/kprocess.h ../libc/include/libio.h ../libc/include/sys/types.h
vmm.o: ../libc/include/signal.h ../include/symtable.h
