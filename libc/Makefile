INCLUDES = -I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
SRCS = *.c */*.c

all: libc

libc: stdio/stdio.o stdio/fopen.o stdio/fwrite.o stdio/fmemopen.o stdlib.o string.o unistd.o time.o ctype.o errno.o stdio/scanf.o stdio/fprintf.o stdio/printf.o stdio/put.o stdio/sprintf.o stdio/get.o libio.o fcntl.o stdio/fread.o stdio/fseek.o malloc.o start.o stdio/stdfiles.o process.o elf.o signal.o stdio/getline.o
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

ctype.o: include/ctype.h
elf.o: ../include/elf.h include/libio.h include/fcntl.h ../include/types.h
elf.o: include/stdio.h include/stdarg.h include/string.h
fcntl.o: ../include/types.h include/fcntl.h ../include/syscall.h
libio.o: include/libio.h include/fcntl.h ../include/types.h include/stdio.h
libio.o: include/stdarg.h
malloc.o: include/stdio.h include/fcntl.h include/libio.h ../include/types.h
malloc.o: include/stdarg.h ../include/process.h ../include/pagination.h
malloc.o: ../include/vmm.h ../include/signal.h ../include/syscall.h
malloc.o: include/malloc.h
process.o: ../include/types.h include/stdlib.h include/malloc.h
process.o: include/stdio.h include/fcntl.h include/libio.h include/stdarg.h
process.o: ../include/syscall.h ../include/process.h ../include/pagination.h
process.o: ../include/vmm.h ../include/signal.h ../include/elf.h
signal.o: ../include/signal.h ../include/types.h ../include/syscall.h
start.o: ../include/process.h include/libio.h include/fcntl.h
start.o: ../include/types.h ../include/pagination.h ../include/vmm.h
start.o: ../include/signal.h
stdlib.o: include/stdio.h include/fcntl.h include/libio.h ../include/types.h
stdlib.o: include/stdarg.h include/stdlib.h include/malloc.h
stdlib.o: ../include/memory.h ../include/pagination.h ../include/heap.h
stdlib.o: ../include/errno.h include/ctype.h include/string.h
string.o: include/stdlib.h ../include/types.h include/malloc.h
string.o: include/string.h include/ctype.h
time.o: include/time.h ../include/types.h include/stdio.h include/fcntl.h
time.o: include/libio.h include/stdarg.h ../include/clock.h
unistd.o: include/unistd.h ../include/types.h ../include/events.h
unistd.o: include/time.h include/stdio.h include/fcntl.h include/libio.h
unistd.o: include/stdarg.h ../include/syscall.h
stdio/fmemopen.o: include/stdio.h include/fcntl.h include/libio.h
stdio/fmemopen.o: ../include/types.h include/stdarg.h include/stdlib.h
stdio/fmemopen.o: include/malloc.h include/fopen.h
stdio/fopen.o: include/stdio.h include/fcntl.h include/libio.h
stdio/fopen.o: ../include/types.h include/stdarg.h include/stdlib.h
stdio/fopen.o: include/malloc.h include/string.h ../include/process.h
stdio/fopen.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
stdio/fprintf.o: include/stdio.h include/fcntl.h include/libio.h
stdio/fprintf.o: ../include/types.h include/stdarg.h include/string.h
stdio/fread.o: include/stdio.h include/fcntl.h include/libio.h
stdio/fread.o: ../include/types.h include/stdarg.h
stdio/fseek.o: include/stdio.h include/fcntl.h include/libio.h
stdio/fseek.o: ../include/types.h include/stdarg.h
stdio/fwrite.o: include/stdio.h include/fcntl.h include/libio.h
stdio/fwrite.o: ../include/types.h include/stdarg.h
stdio/get.o: include/stdio.h include/fcntl.h include/libio.h
stdio/get.o: ../include/types.h include/stdarg.h include/stdlib.h
stdio/get.o: include/malloc.h include/unistd.h
stdio/printf.o: include/stdio.h include/fcntl.h include/libio.h
stdio/printf.o: ../include/types.h include/stdarg.h
stdio/put.o: include/stdio.h include/fcntl.h include/libio.h
stdio/put.o: ../include/types.h include/stdarg.h include/stdlib.h
stdio/put.o: include/malloc.h include/unistd.h
stdio/scanf.o: include/ctype.h include/stdarg.h include/stdlib.h
stdio/scanf.o: ../include/types.h include/malloc.h include/stdio.h
stdio/scanf.o: include/fcntl.h include/libio.h include/string.h
stdio/sprintf.o: include/stdio.h include/fcntl.h include/libio.h
stdio/sprintf.o: ../include/types.h include/stdarg.h
stdio/stdfiles.o: ../include/types.h include/libio.h include/fcntl.h
stdio/stdfiles.o: include/string.h include/stdlib.h include/malloc.h
stdio/stdio.o: include/stdio.h include/fcntl.h include/libio.h
stdio/stdio.o: ../include/types.h include/stdarg.h include/stdlib.h
stdio/stdio.o: include/malloc.h
