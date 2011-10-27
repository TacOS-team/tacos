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
heap.o: ../libc/include/sys/types.h ../include/debug.h
list.o: ../include/list.h ../include/types.h ../libc/include/string.h
list.o: ../libc/include/sys/types.h ../include/kmalloc.h ../include/kstdio.h
widget.o: ../include/gui.h ../include/types.h include/widget.h
widget.o: ../libc/include/stdlib.h ../libc/include/sys/types.h
widget.o: ../libc/include/malloc.h ../libc/include/string.h
widget.o: ../libc/include/stdio.h ../libc/include/libio.h
widget.o: ../libc/include/stdarg.h ../include/mouse.h ../libc/include/ctype.h
widget.o: ../include/process.h ../include/vmm.h ../include/signal.h
