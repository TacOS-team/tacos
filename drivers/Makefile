INCLUDES=-I. -I../include/ -I../libc/include/ -I../kernel/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS=*/*.c

all: libdrivers

libdrivers: keyboard/keyboard.o floppy/floppy.o floppy/floppy_interrupt.o floppy/floppy_utils.o floppy/floppy_motor.o floppy/floppy_dma.o beeper/beeper.o pci/pci.o pci/pci_config.o tty/tty.o video/video.o video/console.o video/vga.o ethernet/rtl8139.o
	  $(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f */*.o

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

beeper/beeper.o: ../include/beeper.h ../include/types.h ../include/ioports.h
ethernet/rtl8139.o: ../include/pci.h ../include/types.h
ethernet/rtl8139.o: ../include/pci_types.h ../include/pci_config.h
ethernet/rtl8139.o: ../include/ioports.h ../libc/include/string.h
ethernet/rtl8139.o: ../libc/include/sys/types.h ../include/kstdio.h
ethernet/rtl8139.o: ../include/klog.h
floppy/floppy.o: ../include/ioports.h ../include/kstdio.h
floppy/floppy.o: ../libc/include/string.h ../libc/include/sys/types.h
floppy/floppy.o: ../include/types.h ../include/klog.h
floppy/floppy.o: floppy/floppy_interrupt.h floppy/floppy_utils.h
floppy/floppy.o: floppy/floppy_motor.h floppy/floppy_dma.h
floppy/floppy_dma.o: ../include/types.h ../libc/include/stdio.h
floppy/floppy_dma.o: ../libc/include/libio.h ../libc/include/sys/types.h
floppy/floppy_dma.o: ../libc/include/stdarg.h ../libc/include/string.h
floppy/floppy_dma.o: ../include/ioports.h ../include/klog.h
floppy/floppy_dma.o: ../include/kstdio.h floppy/floppy_motor.h
floppy/floppy_dma.o: floppy/floppy_utils.h floppy/floppy_interrupt.h
floppy/floppy_dma.o: floppy/floppy_dma.h
floppy/floppy_interrupt.o: ../include/types.h ../include/interrupts.h
floppy/floppy_interrupt.o: floppy/floppy_utils.h
floppy/floppy_motor.o: ../include/types.h ../libc/include/stdio.h
floppy/floppy_motor.o: ../libc/include/libio.h ../libc/include/sys/types.h
floppy/floppy_motor.o: ../libc/include/stdarg.h ../include/ioports.h
floppy/floppy_motor.o: floppy/floppy_motor.h floppy/floppy_utils.h
floppy/floppy_utils.o: ../include/ioports.h ../include/types.h
floppy/floppy_utils.o: ../include/klog.h ../include/kstdio.h
floppy/floppy_utils.o: floppy/floppy_interrupt.h floppy/floppy_utils.h
floppy/floppy_utils.o: floppy/floppy_motor.h
keyboard/keyboard.o: ../include/types.h ../libc/include/stdlib.h
keyboard/keyboard.o: ../libc/include/sys/types.h ../libc/include/malloc.h
keyboard/keyboard.o: ../include/ioports.h ../include/keyboard.h
keyboard/keyboard.o: ../kernel/include/kfcntl.h ../libc/include/libio.h
keyboard/keyboard.o: ../kernel/include/kprocess.h ../include/process.h
keyboard/keyboard.o: ../include/vmm.h ../libc/include/signal.h
keyboard/keyboard.o: ../kernel/include/ksyscall.h ../include/console.h
keyboard/keyboard.o: ../include/tty.h
pci/pci.o: ../include/pci.h ../include/types.h ../include/pci_types.h
pci/pci.o: ../include/pci_config.h ../include/klog.h ../include/kstdio.h
pci/pci_config.o: ../include/ioports.h ../include/types.h
pci/pci_config.o: ../include/pci_config.h ../include/pci_types.h
pci/pci_config.o: pci/pci_vendor.h ../libc/include/stdio.h
pci/pci_config.o: ../libc/include/libio.h ../libc/include/sys/types.h
pci/pci_config.o: ../libc/include/stdarg.h
tty/tty.o: ../include/tty.h ../include/types.h ../kernel/include/kfcntl.h
tty/tty.o: ../libc/include/libio.h ../libc/include/sys/types.h
tty/tty.o: ../kernel/include/kprocess.h ../include/process.h ../include/vmm.h
tty/tty.o: ../libc/include/signal.h ../kernel/include/ksyscall.h
tty/tty.o: ../kernel/include/ksem.h ../kernel/include/ksignal.h
tty/tty.o: ../include/kstdio.h ../include/klog.h ../include/kmalloc.h
tty/tty.o: ../libc/include/fcntl.h ../libc/include/termios.h
video/console.o: ../include/console.h ../include/types.h ../include/tty.h
video/console.o: ../kernel/include/kfcntl.h ../libc/include/libio.h
video/console.o: ../libc/include/sys/types.h ../kernel/include/kprocess.h
video/console.o: ../include/process.h ../include/vmm.h
video/console.o: ../libc/include/signal.h ../kernel/include/ksyscall.h
video/console.o: ../include/video.h
video/vga.o: ../include/ioports.h ../include/klog.h ../include/kstdio.h
video/vga.o: ../libc/include/string.h ../libc/include/sys/types.h
video/vga.o: ../include/vga.h ../kernel/include/ksyscall.h ../include/types.h
video/vga.o: ../include/vga_types.h
video/video.o: ../include/types.h ../include/ioports.h
video/video.o: ../libc/include/ctype.h ../include/video.h
video/video.o: ../libc/include/stdlib.h ../libc/include/sys/types.h
video/video.o: ../libc/include/malloc.h ../include/kmalloc.h
