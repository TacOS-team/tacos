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
floppy/floppy.o: ../include/types.h ../include/libio.h ../include/stdarg.h
floppy/floppy.o: floppy/floppy_interrupt.h floppy/floppy_utils.h
floppy/floppy.o: floppy/floppy_motor.h floppy/floppy_dma.h
floppy/floppy_dma.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
floppy/floppy_dma.o: ../include/libio.h ../include/stdarg.h
floppy/floppy_dma.o: ../include/string.h ../include/ioports.h
floppy/floppy_dma.o: floppy/floppy_motor.h floppy/floppy_utils.h
floppy/floppy_dma.o: floppy/floppy_interrupt.h floppy/floppy_dma.h
floppy/floppy_interrupt.o: ../include/types.h ../include/interrupts.h
floppy/floppy_interrupt.o: ../include/stdio.h ../include/fcntl.h
floppy/floppy_interrupt.o: ../include/libio.h ../include/stdarg.h
floppy/floppy_interrupt.o: floppy/floppy_utils.h
floppy/floppy_motor.o: ../include/types.h ../include/stdio.h
floppy/floppy_motor.o: ../include/fcntl.h ../include/libio.h
floppy/floppy_motor.o: ../include/stdarg.h ../include/ioports.h
floppy/floppy_motor.o: floppy/floppy_motor.h floppy/floppy_utils.h
floppy/floppy_utils.o: ../include/ioports.h ../include/stdio.h
floppy/floppy_utils.o: ../include/fcntl.h ../include/types.h
floppy/floppy_utils.o: ../include/libio.h ../include/stdarg.h
floppy/floppy_utils.o: floppy/floppy_interrupt.h floppy/floppy_utils.h
floppy/floppy_utils.o: floppy/floppy_motor.h
keyboard/keyboard.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
keyboard/keyboard.o: ../include/libio.h ../include/stdarg.h
keyboard/keyboard.o: ../include/stdlib.h ../include/ioports.h
keyboard/keyboard.o: ../include/keyboard.h ../include/process.h
keyboard/keyboard.o: ../include/pagination.h ../include/video.h
mouse/mouse.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
mouse/mouse.o: ../include/libio.h ../include/stdarg.h ../include/ioports.h
mouse/mouse.o: ../include/mouse.h ../include/interrupts.h
serial/serial.o: ../include/interrupts.h ../include/types.h
serial/serial.o: ../include/ioports.h ../include/serial.h ../include/stdio.h
serial/serial.o: ../include/fcntl.h ../include/libio.h ../include/stdarg.h
serial/serial.o: serial/serial_masks.h
