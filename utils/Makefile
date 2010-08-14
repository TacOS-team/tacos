INCLUDES=-I../include/ -Iinclude/ -I../libc/include/
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
heap.o: ../include/debug.h
list.o: ../include/list.h ../include/types.h ../libc/include/string.h
list.o: ../libc/include/stdlib.h ../libc/include/stdio.h
list.o: ../libc/include/fcntl.h ../include/kfcntl.h ../libc/include/libio.h
list.o: ../libc/include/stdarg.h
widget.o: ../include/gui.h ../include/types.h include/widget.h
widget.o: ../libc/include/stdlib.h ../libc/include/string.h
widget.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
widget.o: ../libc/include/libio.h ../libc/include/stdarg.h ../include/mouse.h
widget.o: ../libc/include/ctype.h ../include/process.h
widget.o: ../include/pagination.h ../include/vmm.h
