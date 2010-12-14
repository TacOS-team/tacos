INCLUDES=-I../include/ -Iinclude/ -I../libc/include/ -I../kernel/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
 
SRCS=*.c

all: libutils

libutils: heap.o list.o
	$(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

heap.o: ../include/heap.h ../include/types.h ../libc/include/string.h
heap.o: ../include/debug.h
list.o: ../include/list.h ../include/types.h ../libc/include/string.h
list.o: ../include/kmalloc.h ../kernel/include/kstdio.h
widget.o: ../include/gui.h ../include/types.h include/widget.h
widget.o: ../libc/include/stdlib.h ../libc/include/malloc.h
widget.o: ../libc/include/string.h ../libc/include/stdio.h
widget.o: ../libc/include/fcntl.h ../libc/include/libio.h
widget.o: ../libc/include/stdarg.h ../include/mouse.h ../libc/include/ctype.h
widget.o: ../include/process.h ../include/pagination.h ../include/vmm.h
widget.o: ../include/signal.h
