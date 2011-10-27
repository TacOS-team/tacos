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
dirent.o: include/dirent.h include/sys/types.h include/sys/stat.h
dirent.o: include/stdlib.h include/malloc.h include/string.h
dirent.o: include/sys/syscall.h include/unistd.h
elf.o: ../include/elf.h include/libio.h include/sys/types.h include/stdio.h
elf.o: include/stdarg.h include/string.h include/stdlib.h include/malloc.h
fcntl.o: include/fcntl.h include/unistd.h include/sys/types.h
fcntl.o: include/sys/syscall.h
ioctl.o: include/sys/ioctl.h include/sys/syscall.h include/sys/types.h
ioctl.o: include/unistd.h
libio.o: include/libio.h include/sys/types.h include/stdio.h include/stdarg.h
malloc.o: include/stdio.h include/libio.h include/sys/types.h
malloc.o: include/stdarg.h ../include/process.h ../include/types.h
malloc.o: ../include/vmm.h ../include/signal.h include/unistd.h
malloc.o: include/sys/syscall.h include/malloc.h
process.o: include/sys/types.h include/stdlib.h include/malloc.h
process.o: include/stdio.h include/libio.h include/stdarg.h include/unistd.h
process.o: include/sys/syscall.h ../include/process.h ../include/types.h
process.o: ../include/vmm.h ../include/signal.h ../include/elf.h
process.o: include/string.h
signal.o: ../include/signal.h ../include/types.h include/unistd.h
signal.o: include/sys/types.h include/sys/syscall.h ../include/process.h
signal.o: include/libio.h ../include/vmm.h
start.o: include/libio.h include/sys/types.h ../include/process.h
start.o: ../include/types.h ../include/vmm.h ../include/signal.h
start.o: include/stdio.h include/stdarg.h include/stdlib.h include/malloc.h
stdlib.o: include/stdio.h include/libio.h include/sys/types.h
stdlib.o: include/stdarg.h include/stdlib.h include/malloc.h
stdlib.o: ../include/heap.h ../include/types.h include/errno.h
stdlib.o: include/ctype.h include/string.h
string.o: include/stdlib.h include/sys/types.h include/malloc.h
string.o: include/string.h include/ctype.h
termios.o: include/termios.h include/sys/ioctl.h
time.o: include/time.h include/sys/types.h include/stdio.h include/libio.h
time.o: include/stdarg.h include/stdlib.h include/malloc.h include/unistd.h
time.o: include/sys/syscall.h
unistd.o: include/dirent.h include/sys/types.h include/sys/stat.h
unistd.o: include/errno.h include/stdio.h include/libio.h include/stdarg.h
unistd.o: include/stdlib.h include/malloc.h include/string.h
unistd.o: include/sys/syscall.h include/unistd.h
stdio/fclose.o: include/fcntl.h ../include/process.h include/libio.h
stdio/fclose.o: include/sys/types.h ../include/types.h ../include/vmm.h
stdio/fclose.o: ../include/signal.h
stdio/fmemopen.o: include/stdio.h include/libio.h include/sys/types.h
stdio/fmemopen.o: include/stdarg.h include/stdlib.h include/malloc.h
stdio/fmemopen.o: include/fopen.h
stdio/fopen.o: include/stdio.h include/libio.h include/sys/types.h
stdio/fopen.o: include/stdarg.h include/stdlib.h include/malloc.h
stdio/fopen.o: include/string.h include/fcntl.h ../include/process.h
stdio/fopen.o: ../include/types.h ../include/vmm.h ../include/signal.h
stdio/fopen.o: include/unistd.h
stdio/fprintf.o: include/stdio.h include/libio.h include/sys/types.h
stdio/fprintf.o: include/stdarg.h include/string.h
stdio/fread.o: include/stdio.h include/libio.h include/sys/types.h
stdio/fread.o: include/stdarg.h
stdio/fseek.o: include/stdio.h include/libio.h include/sys/types.h
stdio/fseek.o: include/stdarg.h include/unistd.h
stdio/fwrite.o: include/stdio.h include/libio.h include/sys/types.h
stdio/fwrite.o: include/stdarg.h
stdio/get.o: include/stdio.h include/libio.h include/sys/types.h
stdio/get.o: include/stdarg.h include/stdlib.h include/malloc.h
stdio/get.o: include/unistd.h
stdio/getline.o: include/stdio.h include/libio.h include/sys/types.h
stdio/getline.o: include/stdarg.h
stdio/printf.o: include/stdio.h include/libio.h include/sys/types.h
stdio/printf.o: include/stdarg.h
stdio/put.o: include/stdio.h include/libio.h include/sys/types.h
stdio/put.o: include/stdarg.h include/stdlib.h include/malloc.h
stdio/put.o: include/unistd.h
stdio/scanf.o: include/ctype.h include/stdarg.h include/stdlib.h
stdio/scanf.o: include/sys/types.h include/malloc.h include/stdio.h
stdio/scanf.o: include/libio.h include/string.h
stdio/sprintf.o: include/stdio.h include/libio.h include/sys/types.h
stdio/sprintf.o: include/stdarg.h
stdio/stdfiles.o: include/sys/types.h include/libio.h include/string.h
stdio/stdfiles.o: include/stdlib.h include/malloc.h
stdio/stdio.o: include/stdio.h include/libio.h include/sys/types.h
stdio/stdio.o: include/stdarg.h include/stdlib.h include/malloc.h
