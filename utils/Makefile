INCLUDE=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
  
all: libutils

libutils: heap.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE) 

heap.o : ../include/heap.h ../include/types.h

clean:
	rm -f *.o

