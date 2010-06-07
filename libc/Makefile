INCLUDES = -I../include/ -I../kernel/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
SRCS = *.c */*.c

all: libc

libc: stdio/stdio.o stdio/video.o stdio/fwrite.o stdio/fmemopen.o stdlib.o string.o unistd.o time.o ctype.o errno.o stdio/scanf.o stdio/stdfiles.o stdio/fprintf.o stdio/printf.o stdio/put.o stdio/sprintf.o stdio/get.o libio.o fcntl.o
	$(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o */*.o

mrproper: clean
	@rm ../lib/libc.a

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

ctype.o: ../include/ctype.h
fcntl.o: ../include/types.h ../include/fcntl.h ../include/libio.h
fcntl.o: ../include/process.h ../include/pagination.h ../include/stdio.h
fcntl.o: ../include/stdarg.h ../include/string.h ../include/syscall.h
fcntl.o: ../include/fat.h ../include/video.h ../include/stdlib.h
libio.o: ../include/libio.h ../include/fcntl.h ../include/types.h
libio.o: ../include/stdio.h ../include/stdarg.h
stdlib.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdlib.o: ../include/libio.h ../include/stdarg.h ../include/stdlib.h
stdlib.o: ../include/memory.h ../include/pagination.h ../include/heap.h
stdlib.o: ../include/errno.h ../include/ctype.h ../include/string.h
stdlib.o: ../include/kmalloc.h
string.o: ../include/stdlib.h ../include/types.h ../include/string.h
string.o: ../include/ctype.h
time.o: ../include/time.h ../include/types.h ../include/stdio.h
time.o: ../include/fcntl.h ../include/libio.h ../include/stdarg.h
time.o: ../include/clock.h
unistd.o: ../include/unistd.h ../include/types.h ../include/events.h
unistd.o: ../include/time.h ../include/process.h ../include/libio.h
unistd.o: ../include/fcntl.h ../include/pagination.h ../include/stdio.h
unistd.o: ../include/stdarg.h ../include/syscall.h
stdio/fmemopen.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/fmemopen.o: ../include/libio.h ../include/stdarg.h ../include/stdlib.h
stdio/fmemopen.o: ../include/fopen.h
stdio/fopen.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/fopen.o: ../include/libio.h ../include/stdarg.h ../include/string.h
stdio/fprintf.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/fprintf.o: ../include/libio.h ../include/stdarg.h ../include/string.h
stdio/fwrite.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/fwrite.o: ../include/libio.h ../include/stdarg.h
stdio/get.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/get.o: ../include/libio.h ../include/stdarg.h
stdio/printf.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/printf.o: ../include/libio.h ../include/stdarg.h
stdio/put.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/put.o: ../include/libio.h ../include/stdarg.h ../include/stdlib.h
stdio/put.o: ../include/unistd.h
stdio/scanf.o: ../include/ctype.h ../include/stdarg.h ../include/stdlib.h
stdio/scanf.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
stdio/scanf.o: ../include/libio.h ../include/string.h
stdio/sprintf.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/sprintf.o: ../include/libio.h ../include/stdarg.h
stdio/stdfiles.o: ../include/types.h ../include/libio.h ../include/fcntl.h
stdio/stdfiles.o: ../include/string.h ../include/kmalloc.h
stdio/stdio.o: ../include/video.h ../include/types.h ../include/fcntl.h
stdio/stdio.o: ../include/libio.h
stdio/video.o: ../include/types.h ../include/ioports.h ../include/ctype.h
stdio/video.o: ../include/video.h ../include/stdlib.h ../include/syscall.h
stdio/video.o: ../include/process.h ../include/libio.h ../include/fcntl.h
stdio/video.o: ../include/pagination.h
