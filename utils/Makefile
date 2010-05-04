INCLUDES=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
 
SRCS=heap.c

all: libutils

libutils: heap.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o

depend: $(SRCS)
		makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

heap.o: ../include/heap.h ../include/types.h ../include/string.h
heap.o: ../include/debug.h
