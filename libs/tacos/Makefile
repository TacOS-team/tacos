INCLUDES = -I../../include/ -I../libc/include/ -I.
CFLAGS=-O3 -W -Wall $(WE) -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
SRCS = *.c

all: libtacos

libtacos: libtacos.o
	$(AR) r ../../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

clean:
	rm -f *.o .depend

mrproper: clean
	@rm ../../lib/libtacos.a

-include .depend

depend: $(SRCS)
	  $(CC) -MM $(CFLAGS) $(INCLUDES) $^ > .depend
