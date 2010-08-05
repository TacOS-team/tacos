INCLUDES=-I. -I../include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS=*/*.c

all: libdrivers

libdrivers: mouse/mouse.o keyboard/keyboard.o floppy/floppy.o floppy/floppy_interrupt.o floppy/floppy_utils.o floppy/floppy_motor.o floppy/floppy_dma.o beeper/beeper.o serial/serial.o
	  $(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f */*.o

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

beeper/beeper.o: ../include/beeper.h ../include/types.h ../include/ioports.h
floppy/floppy.o: ../include/ioports.h ../include/stdio.h ../include/fcntl.h
floppy/floppy.o: ../include/types.h ../include/libio.h ../include/kfcntl.h
floppy/floppy.o: ../include/stdarg.h floppy/floppy_interrupt.h
floppy/floppy.o: floppy/floppy_utils.h floppy/floppy_motor.h
floppy/floppy.o: floppy/floppy_dma.h
floppy/floppy_dma.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
floppy/floppy_dma.o: ../include/libio.h ../include/kfcntl.h
floppy/floppy_dma.o: ../include/stdarg.h ../include/string.h
floppy/floppy_dma.o: ../include/ioports.h floppy/floppy_motor.h
floppy/floppy_dma.o: floppy/floppy_utils.h floppy/floppy_interrupt.h
floppy/floppy_dma.o: floppy/floppy_dma.h
floppy/floppy_interrupt.o: ../include/types.h ../include/interrupts.h
floppy/floppy_interrupt.o: ../include/stdio.h ../include/fcntl.h
floppy/floppy_interrupt.o: ../include/libio.h ../include/kfcntl.h
floppy/floppy_interrupt.o: ../include/stdarg.h floppy/floppy_utils.h
floppy/floppy_motor.o: ../include/types.h ../include/stdio.h
floppy/floppy_motor.o: ../include/fcntl.h ../include/libio.h
floppy/floppy_motor.o: ../include/kfcntl.h ../include/stdarg.h
floppy/floppy_motor.o: ../include/ioports.h floppy/floppy_motor.h
floppy/floppy_motor.o: floppy/floppy_utils.h
floppy/floppy_utils.o: ../include/ioports.h ../include/stdio.h
floppy/floppy_utils.o: ../include/fcntl.h ../include/types.h
floppy/floppy_utils.o: ../include/libio.h ../include/kfcntl.h
floppy/floppy_utils.o: ../include/stdarg.h floppy/floppy_interrupt.h
floppy/floppy_utils.o: floppy/floppy_utils.h floppy/floppy_motor.h
keyboard/keyboard.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
keyboard/keyboard.o: ../include/libio.h ../include/kfcntl.h
keyboard/keyboard.o: ../include/stdarg.h ../include/stdlib.h
keyboard/keyboard.o: ../include/ioports.h ../include/keyboard.h
keyboard/keyboard.o: ../include/process.h ../include/pagination.h
mouse/mouse.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
mouse/mouse.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
mouse/mouse.o: ../include/ioports.h ../include/mouse.h
mouse/mouse.o: ../include/interrupts.h
serial/serial.o: ../include/interrupts.h ../include/types.h
serial/serial.o: ../include/ioports.h ../include/serial.h ../include/stdio.h
serial/serial.o: ../include/fcntl.h ../include/libio.h ../include/kfcntl.h
serial/serial.o: ../include/stdarg.h serial/serial_masks.h
