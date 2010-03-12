INCLUDES=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc

all: libclock

libclock: clock.o i8254.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clock.c: ../include/clock.h include/i8254.h ../include/interrupts.h ../include/types.h ../include/heap.h

i8254.c: include/i8254.h ../include/types.h

clean:
	rm -f *.o

