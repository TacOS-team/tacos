INCLUDES = -I../include/ -Iinclude/
CFLAGS=-mno-sse -mno-mmx -W -Wall $(WE) -g -nostdlib -nostdinc -fno-builtin -I`pwd` -m32
SRCS = *.c */*.c

all: libc

libc: stdio/stdio.o stdio/rename.o stdio/fopen.o stdio/fwrite.o stdio/fmemopen.o termios.o dirent.o stdlib.o string.o unistd.o time.o ctype.o errno.o stdio/scanf.o stdio/fprintf.o stdio/printf.o stdio/put.o stdio/sprintf.o stdio/get.o libio.o fcntl.o stdio/fread.o stdio/fseek.o stdio/fgetpos.o malloc.o start.o stdio/stdfiles.o process.o signal.o stdio/getline.o stdio/fclose.o stdio/remove.o stdio/ferr.o stdio/perror.o ioctl.o wait.o sem.o select.o mman.o sysdeps/tacos/syscall.o
	$(AR) r ../../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o */*.o .depend

mrproper: clean
	@rm ../../lib/libc.a

-include .depend

depend: $(SRCS)
	  $(CC) -MM $(CFLAGS) $(INCLUDES) $^ > .depend
