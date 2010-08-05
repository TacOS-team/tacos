INCLUDES=-I../include/ -Iinclude/
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

heap.o: ../include/heap.h ../include/types.h ../include/string.h
heap.o: ../include/debug.h
list.o: ../include/list.h ../include/types.h ../include/string.h
list.o: ../include/stdlib.h ../include/stdio.h ../include/fcntl.h
list.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
widget.o: ../include/gui.h ../include/types.h include/widget.h
widget.o: ../include/stdlib.h ../include/string.h ../include/stdio.h
widget.o: ../include/fcntl.h ../include/libio.h ../include/kfcntl.h
widget.o: ../include/stdarg.h ../include/mouse.h ../include/video.h
widget.o: ../include/ctype.h ../include/process.h ../include/pagination.h
