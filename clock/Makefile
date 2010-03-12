INCLUDES=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc

all: libclock

libclock: clock.o events.o i8254.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clock.c: ../include/clock.h include/i8254.h ../include/types.h 

i8254.c: include/i8254.h ../include/types.h

events.c : ../include/events.h ../include/clock.h include/i8254.h

clean:
	rm -f *.o

