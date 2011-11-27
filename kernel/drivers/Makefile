INCLUDES=-I. -I../../include/ -I../../libc/include/ -I../include/
CFLAGS=-W -Wall $(WE) -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS=*/*.c

all: libdrivers

libdrivers: char/dummy_driver.o char/serial.o char/mouse.o block/floppy/floppy.o block/floppy/floppy_interrupt.o block/floppy/floppy_utils.o block/floppy/floppy_motor.o block/floppy/floppy_dma.o char/tty/tty.o char/tty/console.o char/tty/keyboard.o
	  $(AR) r ../../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f char/*.o .depend

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $^ > .depend
