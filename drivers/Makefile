INCLUDES=-I. -I../include/
LDFLAGS=-L../lib/
LDLIBS=-lc

all: libdrivers

libdrivers: mouse/mouse.o keyboard/keyboard.o floppy/floppy.o floppy/floppy_interrupt.o floppy/floppy_utils.o floppy/floppy_motor.o floppy/floppy_dma.o beeper/beeper.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

floppy: force_look
	$(MAKE) -C $@ 

mouse: force_look
	$(MAKE) -C $@ 

keyboard: force_look
	$(MAKE) -C $@ 

beeper: force_look
	$(MAKE) -C $@
	
clean:
	rm -f *.o

force_look:
	@true
