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
fat.o: ../include/types.h include/kfat.h include/kfcntl.h include/fd_types.h
fat.o: include/kprocess.h include/ksyscall.h ../include/process.h
fat.o: ../include/elf.h include/signal_types.h include/process_types.h
fat.o: ../include/symtable.h ../include/vmm.h ../libc/include/time.h
fat.o: ../libc/include/sys/types.h ../libc/include/sys/stat.h
fat.o: ../include/klog.h ../include/kstdio.h ../include/kmalloc.h
fat.o: ../libc/include/string.h include/clock.h
fpu.o: include/fpu.h ../include/interrupts.h ../include/types.h
fpu.o: ../include/klog.h ../include/kstdio.h
gdt.o: ../include/types.h include/memory.h include/gdt.h
i8254.o: include/i8254.h ../include/types.h ../include/ioports.h
i8259.o: ../include/types.h ../include/ioports.h
idt.o: ../include/types.h include/idt.h
init.o: include/kdriver.h ../include/types.h include/kfcntl.h
init.o: include/fd_types.h include/kprocess.h include/ksyscall.h
init.o: ../include/process.h ../include/elf.h include/signal_types.h
init.o: include/process_types.h ../include/symtable.h ../include/vmm.h
init.o: ../include/klog.h ../include/kstdio.h include/scheduler.h
init.o: include/drivers/dummy_driver.h include/drivers/mouse.h
init.o: ../libc/include/stdio.h ../libc/include/libio.h
init.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
init.o: ../libc/include/stdlib.h ../libc/include/malloc.h
interrupts.o: ../include/types.h include/idt.h include/i8259.h
interrupts.o: ../include/interrupts.h
kdirent.o: include/kdirent.h include/ksyscall.h ../include/types.h
kdirent.o: ../libc/include/dirent.h ../libc/include/sys/types.h
kdirent.o: ../libc/include/sys/stat.h include/kfat.h include/kfcntl.h
kdirent.o: include/fd_types.h include/kprocess.h ../include/process.h
kdirent.o: ../include/elf.h include/signal_types.h include/process_types.h
kdirent.o: ../include/symtable.h ../include/vmm.h ../libc/include/time.h
kdirent.o: ../include/kmalloc.h ../libc/include/string.h
kdriver.o: include/kdriver.h ../include/types.h include/kfcntl.h
kdriver.o: include/fd_types.h include/kprocess.h include/ksyscall.h
kdriver.o: ../include/process.h ../include/elf.h include/signal_types.h
kdriver.o: include/process_types.h ../include/symtable.h ../include/vmm.h
kdriver.o: ../libc/include/string.h ../libc/include/sys/types.h
kernel.o: include/multiboot.h include/idt.h ../include/types.h
kernel.o: include/i8259.h include/kunistd.h include/ksyscall.h
kernel.o: ../libc/include/string.h ../libc/include/sys/types.h
kernel.o: include/memory.h include/pagination.h include/gdt.h
kernel.o: include/exception.h ../include/interrupts.h ../include/pci.h
kernel.o: ../include/pci_types.h ../include/pci_config.h include/scheduler.h
kernel.o: include/kprocess.h include/fd_types.h ../include/process.h
kernel.o: ../include/elf.h include/signal_types.h include/process_types.h
kernel.o: ../include/symtable.h ../include/vmm.h include/clock.h
kernel.o: ../libc/include/time.h ../include/keyboard.h include/kfcntl.h
kernel.o: include/events.h ../include/floppy.h include/kpanic.h
kernel.o: ../include/kmalloc.h ../include/ioports.h include/msr.h
kernel.o: include/kfat.h ../libc/include/sys/stat.h include/fpu.h
kernel.o: include/vm86.h ../include/beeper.h ../include/debug.h
kernel.o: ../include/shell.h ../libc/include/sys/syscall.h include/ksem.h
kernel.o: ../include/video.h ../include/console.h ../include/tty.h
kernel.o: include/ksignal.h ../libc/include/signal.h include/round_robin.h
kernel.o: ../include/rtl8139.h ../include/klog.h ../include/kstdio.h
kernel.o: include/kdriver.h include/kdirent.h ../libc/include/dirent.h
kernel.o: ../include/vga.h ../include/vga_types.h include/init.h
kernel.o: include/drivers/dummy_driver.h include/drivers/serial.h
kernel.o: include/drivers/mouse.h
kfcntl.o: include/kprocess.h include/ksyscall.h ../include/types.h
kfcntl.o: include/fd_types.h ../include/process.h ../include/elf.h
kfcntl.o: include/signal_types.h include/process_types.h
kfcntl.o: ../include/symtable.h ../include/vmm.h include/scheduler.h
kfcntl.o: include/kfat.h include/kfcntl.h ../libc/include/time.h
kfcntl.o: ../libc/include/sys/types.h ../libc/include/sys/stat.h
kfcntl.o: ../libc/include/string.h ../include/kmalloc.h include/kdriver.h
kfcntl.o: ../include/keyboard.h ../include/console.h ../include/tty.h
kfcntl.o: ../include/klog.h ../include/kstdio.h ../libc/include/fcntl.h
kmalloc.o: ../include/types.h include/memory.h ../include/vmm.h
kmalloc.o: ../include/kmalloc.h
kpanic.o: ../include/kstdio.h include/exception.h ../include/types.h
kpanic.o: ../include/interrupts.h include/kpanic.h include/kprocess.h
kpanic.o: include/ksyscall.h include/fd_types.h ../include/process.h
kpanic.o: ../include/elf.h include/signal_types.h include/process_types.h
kpanic.o: ../include/symtable.h ../include/vmm.h include/scheduler.h
kpanic.o: ../include/console.h ../include/tty.h include/kfcntl.h
kprocess.o: ../include/debug.h ../include/elf.h ../include/types.h
kprocess.o: include/gdt.h include/kfcntl.h include/fd_types.h
kprocess.o: include/kprocess.h include/ksyscall.h ../include/process.h
kprocess.o: include/signal_types.h include/process_types.h
kprocess.o: ../include/symtable.h ../include/vmm.h ../include/klog.h
kprocess.o: ../include/kstdio.h ../include/kmalloc.h include/scheduler.h
kprocess.o: ../libc/include/string.h ../libc/include/sys/types.h
kprocess.o: include/pagination.h
ksem.o: include/ksem.h include/ksyscall.h ../include/types.h
ksem.o: include/kprocess.h include/fd_types.h ../include/process.h
ksem.o: ../include/elf.h include/signal_types.h include/process_types.h
ksem.o: ../include/symtable.h ../include/vmm.h ../include/kmalloc.h
ksem.o: include/scheduler.h
ksignal.o: include/gdt.h ../include/types.h include/ksignal.h
ksignal.o: include/ksyscall.h include/signal_types.h ../libc/include/signal.h
ksignal.o: ../libc/include/sys/types.h include/kprocess.h include/fd_types.h
ksignal.o: ../include/process.h ../include/elf.h include/process_types.h
ksignal.o: ../include/symtable.h ../include/vmm.h ../include/kstdio.h
ksignal.o: include/pagination.h include/scheduler.h ../include/klog.h
ksignal.o: ../include/interrupts.h ../libc/include/sys/syscall.h
kstdio.o: ../libc/include/fcntl.h include/kfcntl.h ../include/types.h
kstdio.o: include/fd_types.h include/kprocess.h include/ksyscall.h
kstdio.o: ../include/process.h ../include/elf.h include/signal_types.h
kstdio.o: include/process_types.h ../include/symtable.h ../include/vmm.h
kstdio.o: ../include/console.h ../include/tty.h
ksyscall.o: include/gdt.h ../include/types.h ../include/interrupts.h
ksyscall.o: include/ksyscall.h ../include/klog.h ../include/kstdio.h
ksyscall.o: include/msr.h ../libc/include/string.h
ksyscall.o: ../libc/include/sys/types.h
kunistd.o: include/kprocess.h include/ksyscall.h ../include/types.h
kunistd.o: include/fd_types.h ../include/process.h ../include/elf.h
kunistd.o: include/signal_types.h include/process_types.h
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
round_robin.o: include/ksyscall.h include/fd_types.h ../include/process.h
round_robin.o: ../include/elf.h include/signal_types.h
round_robin.o: include/process_types.h ../include/symtable.h ../include/vmm.h
round_robin.o: include/scheduler.h
scheduler.o: include/clock.h ../include/types.h ../libc/include/time.h
scheduler.o: ../libc/include/sys/types.h include/ksyscall.h include/context.h
scheduler.o: include/kprocess.h include/fd_types.h ../include/process.h
scheduler.o: ../include/elf.h include/signal_types.h include/process_types.h
scheduler.o: ../include/symtable.h ../include/vmm.h ../include/debug.h
scheduler.o: include/events.h include/gdt.h include/i8259.h ../include/klog.h
scheduler.o: ../include/kstdio.h ../include/kmalloc.h include/ksignal.h
scheduler.o: ../libc/include/signal.h ../include/interrupts.h
scheduler.o: ../include/ioports.h include/scheduler.h include/pagination.h
scheduler.o: ../libc/include/string.h ../libc/include/unistd.h
scheduler.o: ../libc/include/sys/syscall.h
symtable.o: ../libc/include/string.h ../libc/include/sys/types.h
symtable.o: include/kprocess.h include/ksyscall.h ../include/types.h
symtable.o: include/fd_types.h ../include/process.h ../include/elf.h
symtable.o: include/signal_types.h include/process_types.h
symtable.o: ../include/symtable.h ../include/vmm.h ../include/kmalloc.h
symtable.o: ../include/klog.h ../include/kstdio.h ../libc/include/fcntl.h
vm86.o: ../include/debug.h include/gdt.h ../include/types.h
vmm.o: include/memory.h ../include/types.h include/pagination.h
vmm.o: ../include/vmm.h ../include/process.h ../include/elf.h
vmm.o: include/ksyscall.h ../include/kstdio.h include/scheduler.h
vmm.o: include/kprocess.h include/fd_types.h include/signal_types.h
vmm.o: include/process_types.h ../include/symtable.h
