INCLUDES=-I. -I../include/ -I../libs/libc/include/ -I../kernel/include/
CFLAGS=-W -Wall $(WE) -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS=*/*.c

all: libbaddrivers

libbaddrivers: ethernet/rtl8139.o
	  $(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f */*.o .depend

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $^ > .depend
