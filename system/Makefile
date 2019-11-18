INCLUDES=-I../include/ -Iinclude/ -I../libs/libc/include/
CFLAGS=-W -Wall $(WE) -g -nostdlib -nostdinc -fno-builtin -I`pwd` -m32

SRCS=*.c

all: libsystem

libsystem: syscall.o
	$(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o .depend

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $^ > .depend

