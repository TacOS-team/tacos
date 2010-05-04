INCLUDES=-I. -I../include/
LDFLAGS=-L../lib/
LDLIBS=-lc

SUBDIRS=floppy mouse keyboard beeper

all: libdrivers

libdrivers: mouse/mouse.o keyboard/keyboard.o floppy/floppy.o floppy/floppy_interrupt.o floppy/floppy_utils.o floppy/floppy_motor.o floppy/floppy_dma.o beeper/beeper.o
	  $(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f */*.o

force_look:
	@true

depend:
	@for i in $(SUBDIRS); do \
		$(MAKE) -C $$i depend; \
	done

