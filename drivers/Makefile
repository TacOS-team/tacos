INCLUDES=-I. -I../include/ -I../libc/include/ -I../kernel/include/
CFLAGS=-W -Wall $(WE) -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS=*/*.c

all: libbaddrivers

libbaddrivers: keyboard/keyboard.o beeper/beeper.o tty/tty.o video/video.o video/console.o video/vga.o ethernet/rtl8139.o
	  $(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f */*.o .depend

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $^ > .depend
