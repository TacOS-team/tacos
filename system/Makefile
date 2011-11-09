INCLUDES=-I../include/ -Iinclude/ -I../libc/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS=*.c

all: libsystem

libsystem: syscall.o sem.o
	$(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o .depend

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $^ > .depend

