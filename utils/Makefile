INCLUDES=-I../include/ -Iinclude/ -I../libc/include/ -I../kernel/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
 
SRCS=*.c

all: libutils

libutils: heap.o list.o widget.o
	$(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

heap.o: ../include/heap.h ../include/types.h ../libc/include/string.h
heap.o: ../libc/include/sys/types.h ../include/debug.h
list.o: ../include/list.h ../include/types.h ../libc/include/string.h
list.o: ../libc/include/sys/types.h ../include/kmalloc.h ../include/kstdio.h
widget.o: ../libc/include/fcntl.h ../libc/include/unistd.h
widget.o: ../libc/include/sys/types.h ../include/gui.h ../include/types.h
widget.o: include/widget.h ../libc/include/stdlib.h ../libc/include/malloc.h
widget.o: ../libc/include/string.h ../libc/include/stdio.h
widget.o: ../libc/include/libio.h ../libc/include/stdarg.h
widget.o: ../libc/include/ctype.h
