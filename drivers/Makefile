INCLUDES=-I. -I../include/ -I../libc/include/ -I../kernel/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS=*/*.c

all: libdrivers

libdrivers: mouse/mouse.o keyboard/keyboard.o floppy/floppy.o floppy/floppy_interrupt.o floppy/floppy_utils.o floppy/floppy_motor.o floppy/floppy_dma.o beeper/beeper.o pci/pci.o pci/pci_config.o tty/tty.o video/video.o video/console.o ethernet/rtl8139.o
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
ethernet/rtl8139.o: ../kernel/include/kstdio.h ../include/klog.h
floppy/floppy.o: ../include/ioports.h ../libc/include/stdio.h
floppy/floppy.o: ../libc/include/libio.h ../include/types.h
floppy/floppy.o: ../libc/include/stdarg.h ../libc/include/string.h
floppy/floppy.o: ../include/klog.h floppy/floppy_interrupt.h
floppy/floppy.o: floppy/floppy_utils.h floppy/floppy_motor.h
floppy/floppy.o: floppy/floppy_dma.h
floppy/floppy_dma.o: ../include/types.h ../libc/include/stdio.h
floppy/floppy_dma.o: ../libc/include/libio.h ../libc/include/stdarg.h
floppy/floppy_dma.o: ../libc/include/string.h ../include/ioports.h
floppy/floppy_dma.o: ../include/klog.h floppy/floppy_motor.h
floppy/floppy_dma.o: floppy/floppy_utils.h floppy/floppy_interrupt.h
floppy/floppy_dma.o: floppy/floppy_dma.h
floppy/floppy_interrupt.o: ../include/types.h ../include/interrupts.h
floppy/floppy_interrupt.o: ../libc/include/stdio.h ../libc/include/libio.h
floppy/floppy_interrupt.o: ../libc/include/stdarg.h floppy/floppy_utils.h
floppy/floppy_motor.o: ../include/types.h ../libc/include/stdio.h
floppy/floppy_motor.o: ../libc/include/libio.h ../libc/include/stdarg.h
floppy/floppy_motor.o: ../include/ioports.h floppy/floppy_motor.h
floppy/floppy_motor.o: floppy/floppy_utils.h
floppy/floppy_utils.o: ../include/ioports.h ../libc/include/stdio.h
floppy/floppy_utils.o: ../libc/include/libio.h ../include/types.h
floppy/floppy_utils.o: ../libc/include/stdarg.h ../include/klog.h
floppy/floppy_utils.o: floppy/floppy_interrupt.h floppy/floppy_utils.h
floppy/floppy_utils.o: floppy/floppy_motor.h
keyboard/keyboard.o: ../include/types.h ../libc/include/stdio.h
keyboard/keyboard.o: ../libc/include/libio.h ../libc/include/stdarg.h
keyboard/keyboard.o: ../libc/include/stdlib.h ../libc/include/malloc.h
keyboard/keyboard.o: ../include/ioports.h ../include/keyboard.h
keyboard/keyboard.o: ../kernel/include/kfcntl.h ../kernel/include/kprocess.h
keyboard/keyboard.o: ../include/process.h ../include/vmm.h
keyboard/keyboard.o: ../include/signal.h ../kernel/include/ksyscall.h
keyboard/keyboard.o: ../include/console.h ../include/tty.h
mouse/mouse.o: ../include/types.h ../libc/include/stdio.h
mouse/mouse.o: ../libc/include/libio.h ../libc/include/stdarg.h
mouse/mouse.o: ../include/ioports.h ../include/mouse.h
mouse/mouse.o: ../include/interrupts.h
pci/pci.o: ../include/pci.h ../include/types.h ../include/pci_types.h
pci/pci.o: ../include/pci_config.h ../libc/include/stdio.h
pci/pci.o: ../libc/include/libio.h ../libc/include/stdarg.h ../include/klog.h
pci/pci_config.o: ../include/ioports.h ../include/types.h
pci/pci_config.o: ../include/pci_config.h ../include/pci_types.h
pci/pci_config.o: pci/pci_vendor.h ../libc/include/stdio.h
pci/pci_config.o: ../libc/include/libio.h ../libc/include/stdarg.h
tty/tty.o: ../include/tty.h ../include/types.h ../kernel/include/kfcntl.h
tty/tty.o: ../libc/include/libio.h ../kernel/include/kprocess.h
tty/tty.o: ../include/process.h ../include/vmm.h ../include/signal.h
tty/tty.o: ../kernel/include/ksyscall.h ../kernel/include/ksem.h
tty/tty.o: ../kernel/include/ksignal.h ../kernel/include/kstdio.h
tty/tty.o: ../include/errno.h ../include/klog.h ../include/kmalloc.h
tty/tty.o: ../libc/include/fcntl.h
video/console.o: ../include/console.h ../include/types.h ../include/tty.h
video/console.o: ../kernel/include/kfcntl.h ../libc/include/libio.h
video/console.o: ../kernel/include/kprocess.h ../include/process.h
video/console.o: ../include/vmm.h ../include/signal.h
video/console.o: ../kernel/include/ksyscall.h ../include/video.h
video/video.o: ../libc/include/stdio.h ../libc/include/libio.h
video/video.o: ../include/types.h ../libc/include/stdarg.h
video/video.o: ../include/ioports.h ../libc/include/ctype.h
video/video.o: ../include/video.h ../libc/include/stdlib.h
video/video.o: ../libc/include/malloc.h ../include/syscall.h
video/video.o: ../include/kmalloc.h
