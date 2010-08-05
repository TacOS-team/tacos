INCLUDES = -I../include/ -I../kernel/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
SRCS = *.c */*.c

all: libc

libc: stdio/stdio.o stdio/fopen.o stdio/fwrite.o stdio/fmemopen.o stdlib.o string.o unistd.o time.o ctype.o errno.o stdio/scanf.o stdio/stdfiles.o stdio/fprintf.o stdio/printf.o stdio/put.o stdio/sprintf.o stdio/get.o libio.o fcntl.o stdio/fread.o stdio/fseek.o
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
fcntl.o: ../include/syscall.h
libio.o: ../include/libio.h ../include/fcntl.h ../include/types.h
libio.o: ../include/stdio.h ../include/kfcntl.h ../include/stdarg.h
stdlib.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdlib.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdlib.o: ../include/stdlib.h ../include/memory.h ../include/pagination.h
stdlib.o: ../include/heap.h ../include/errno.h ../include/ctype.h
stdlib.o: ../include/string.h ../include/kmalloc.h
string.o: ../include/stdlib.h ../include/types.h ../include/string.h
string.o: ../include/ctype.h
time.o: ../include/time.h ../include/types.h ../include/stdio.h
time.o: ../include/fcntl.h ../include/libio.h ../include/kfcntl.h
time.o: ../include/stdarg.h ../include/clock.h
unistd.o: ../include/unistd.h ../include/types.h ../include/events.h
unistd.o: ../include/time.h ../include/stdio.h ../include/fcntl.h
unistd.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
unistd.o: ../include/syscall.h
stdio/fmemopen.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/fmemopen.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/fmemopen.o: ../include/stdlib.h ../include/fopen.h
stdio/fopen.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/fopen.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/fopen.o: ../include/stdlib.h ../include/string.h ../include/process.h
stdio/fopen.o: ../include/pagination.h
stdio/fprintf.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/fprintf.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/fprintf.o: ../include/string.h
stdio/fread.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/fread.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/fseek.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/fseek.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/fwrite.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/fwrite.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/get.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/get.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/get.o: ../include/stdlib.h ../include/unistd.h
stdio/printf.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/printf.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/put.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/put.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/put.o: ../include/stdlib.h ../include/unistd.h
stdio/scanf.o: ../include/ctype.h ../include/stdarg.h ../include/stdlib.h
stdio/scanf.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
stdio/scanf.o: ../include/libio.h ../include/kfcntl.h ../include/string.h
stdio/sprintf.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/sprintf.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/stdfiles.o: ../include/types.h ../include/libio.h ../include/fcntl.h
stdio/stdfiles.o: ../include/string.h ../include/kmalloc.h
stdio/stdio.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
stdio/stdio.o: ../include/libio.h ../include/kfcntl.h ../include/stdarg.h
stdio/stdio.o: ../include/stdlib.h ../kernel/include/video.h
