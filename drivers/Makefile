INCLUDES=-I. -I../include/
LDFLAGS=-L../lib/
LDLIBS=-lc

all: libdrivers

libdrivers: floppy.o floppy_interrupt.o floppy_utils.o floppy_motor.o floppy_dma.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

floppy.o: ../include/floppy.h



clean:
	rm -f *.o

