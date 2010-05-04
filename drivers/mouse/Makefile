INCLUDES=-I../../include/ -Iinclude/
LDFLAGS=-L../../lib/
LDLIBS=-lc

SRCS=mouse.c

mouse: mouse.o 

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o

depend: $(SRCS)
		makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

mouse.o: ../../include/types.h ../../include/stdio.h ../../include/libio.h
mouse.o: ../../include/fcntl.h ../../include/stdarg.h ../../include/ioports.h
mouse.o: ../../include/mouse.h ../../include/interrupts.h
