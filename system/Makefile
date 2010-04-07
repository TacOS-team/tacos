INCLUDES=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc

all: libsystem

libsystem: syscall.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

syscall.c : ../include/syscall.h

clean:
	rm -f *.o
