INCLUDES=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc

all: libmouse

libmouse: mouse.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

mouse.c: ../include/mouse.h ../include/types.h ../include/ioports.h ../include/stdio.h

clean:
	rm -f *.o

