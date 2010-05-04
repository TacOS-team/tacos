INCLUDES=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc

SRCS=syscall.c

all: libsystem

libsystem: syscall.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o

depend: $(SRCS)
		makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

syscall.o: ../include/syscall.h ../include/types.h
