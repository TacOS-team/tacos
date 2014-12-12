INCLUDES=-Iinclude/ -I../../libc/include/
CFLAGS=-W -Wall $(WE) -g -nostdlib -nostdinc -fno-builtin -I`pwd` -m32 -mno-sse -mno-mmx

SRCS=*.c

all: libutils

libutils: widget.o unittest.o
	$(AR) r ../$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o .depend

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $^ > .depend

