INCLUDES = -I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
SRCS = *.c */*.c

all: libc

libc: stdio/stdio.o stdio/fopen.o stdio/fwrite.o stdio/fmemopen.o termios.o dirent.o stdlib.o string.o unistd.o time.o ctype.o errno.o stdio/scanf.o stdio/fprintf.o stdio/printf.o stdio/put.o stdio/sprintf.o stdio/get.o libio.o fcntl.o stdio/fread.o stdio/fseek.o malloc.o start.o stdio/stdfiles.o process.o elf.o signal.o stdio/getline.o stdio/fclose.o ioctl.o
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
dirent.o: include/dirent.h ../include/types.h include/stdlib.h
dirent.o: include/malloc.h include/string.h ../include/syscall.h
dirent.o: include/unistd.h include/sys/types.h
elf.o: ../include/elf.h include/libio.h ../include/types.h include/stdio.h
elf.o: include/stdarg.h include/string.h include/stdlib.h include/malloc.h
fcntl.o: ../include/types.h include/fcntl.h ../include/syscall.h
ioctl.o: include/sys/ioctl.h ../include/syscall.h ../include/types.h
libio.o: include/libio.h ../include/types.h include/stdio.h include/stdarg.h
malloc.o: include/stdio.h include/libio.h ../include/types.h include/stdarg.h
malloc.o: ../include/process.h ../include/vmm.h ../include/signal.h
malloc.o: ../include/syscall.h include/malloc.h
process.o: ../include/types.h include/stdlib.h include/malloc.h
process.o: include/stdio.h include/libio.h include/stdarg.h
process.o: ../include/syscall.h ../include/process.h ../include/vmm.h
process.o: ../include/signal.h ../include/elf.h include/string.h
signal.o: ../include/signal.h ../include/types.h ../include/syscall.h
signal.o: ../include/process.h include/libio.h ../include/vmm.h
start.o: include/libio.h ../include/types.h ../include/process.h
start.o: ../include/vmm.h ../include/signal.h include/stdio.h
start.o: include/stdarg.h include/stdlib.h include/malloc.h
stdlib.o: include/stdio.h include/libio.h ../include/types.h include/stdarg.h
stdlib.o: include/stdlib.h include/malloc.h ../include/heap.h include/errno.h
stdlib.o: include/ctype.h include/string.h
string.o: include/stdlib.h ../include/types.h include/malloc.h
string.o: include/string.h include/ctype.h
termios.o: include/termios.h include/sys/ioctl.h
time.o: include/time.h ../include/types.h include/stdio.h include/libio.h
time.o: include/stdarg.h
unistd.o: include/dirent.h ../include/types.h include/errno.h include/stdio.h
unistd.o: include/libio.h include/stdarg.h include/stdlib.h include/malloc.h
unistd.o: include/string.h ../include/syscall.h include/unistd.h
unistd.o: include/sys/types.h
stdio/fclose.o: include/fcntl.h ../include/process.h include/libio.h
stdio/fclose.o: ../include/types.h ../include/vmm.h ../include/signal.h
stdio/fmemopen.o: include/stdio.h include/libio.h ../include/types.h
stdio/fmemopen.o: include/stdarg.h include/stdlib.h include/malloc.h
stdio/fmemopen.o: include/fopen.h
stdio/fopen.o: include/stdio.h include/libio.h ../include/types.h
stdio/fopen.o: include/stdarg.h include/stdlib.h include/malloc.h
stdio/fopen.o: include/string.h include/fcntl.h ../include/process.h
stdio/fopen.o: ../include/vmm.h ../include/signal.h include/unistd.h
stdio/fopen.o: include/sys/types.h
stdio/fprintf.o: include/stdio.h include/libio.h ../include/types.h
stdio/fprintf.o: include/stdarg.h include/string.h
stdio/fread.o: include/stdio.h include/libio.h ../include/types.h
stdio/fread.o: include/stdarg.h
stdio/fseek.o: include/stdio.h include/libio.h ../include/types.h
stdio/fseek.o: include/stdarg.h include/unistd.h include/sys/types.h
stdio/fwrite.o: include/stdio.h include/libio.h ../include/types.h
stdio/fwrite.o: include/stdarg.h
stdio/get.o: include/stdio.h include/libio.h ../include/types.h
stdio/get.o: include/stdarg.h include/stdlib.h include/malloc.h
stdio/get.o: include/unistd.h include/sys/types.h
stdio/getline.o: include/stdio.h include/libio.h ../include/types.h
stdio/getline.o: include/stdarg.h
stdio/printf.o: include/stdio.h include/libio.h ../include/types.h
stdio/printf.o: include/stdarg.h
stdio/put.o: include/stdio.h include/libio.h ../include/types.h
stdio/put.o: include/stdarg.h include/stdlib.h include/malloc.h
stdio/put.o: include/unistd.h include/sys/types.h
stdio/scanf.o: include/ctype.h include/stdarg.h include/stdlib.h
stdio/scanf.o: ../include/types.h include/malloc.h include/stdio.h
stdio/scanf.o: include/libio.h include/string.h
stdio/sprintf.o: include/stdio.h include/libio.h ../include/types.h
stdio/sprintf.o: include/stdarg.h
stdio/stdfiles.o: ../include/types.h include/libio.h include/string.h
stdio/stdfiles.o: include/stdlib.h include/malloc.h
stdio/stdio.o: include/stdio.h include/libio.h ../include/types.h
stdio/stdio.o: include/stdarg.h include/stdlib.h include/malloc.h
