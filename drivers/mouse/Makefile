INCLUDES=-I../../include/ -Iinclude/
LDFLAGS=-L../../lib/
LDLIBS=-lc

mouse: mouse.o ../../include/mouse.h ../../include/types.h ../../include/ioports.h ../../include/stdio.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

mouse.c: ../../include/mouse.h ../../include/types.h ../../include/ioports.h ../../include/stdio.h

clean:
	rm -f *.o

