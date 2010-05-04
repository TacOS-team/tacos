INCLUDES=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc

SRCS = clock.c events.c i8254.c

all: libclock

libclock: clock.o events.o i8254.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o
	
depend: $(SRCS)
		makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

clock.o: ../include/types.h ../include/ioports.h include/i8254.h
clock.o: ../include/clock.h ../include/time.h ../include/interrupts.h
clock.o: ../include/heap.h
events.o: ../include/types.h include/i8254.h ../include/interrupts.h
events.o: ../include/time.h ../include/clock.h ../include/heap.h
events.o: ../include/events.h ../include/stdio.h ../include/libio.h
events.o: ../include/fcntl.h ../include/stdarg.h ../include/debug.h
i8254.o: include/i8254.h ../include/types.h ../include/ioports.h
