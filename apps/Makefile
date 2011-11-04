INCLUDES=-I../include/ -Iinclude/ -I../libc/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: shell.o tests.o shell_utils.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o
	
depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE

noxeyes.o: ../libc/include/stdio.h ../libc/include/libio.h
noxeyes.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
noxeyes.o: ../libc/include/stdlib.h ../libc/include/malloc.h
noxeyes.o: ../include/process.h ../include/types.h ../include/elf.h
shell.o: ../libc/include/dirent.h ../libc/include/sys/types.h
shell.o: ../libc/include/sys/stat.h ../include/floppy.h ../include/types.h
shell.o: ../include/kmalloc.h ../include/pci.h ../include/pci_types.h
shell.o: ../include/process.h ../include/elf.h ../libc/include/stdio.h
shell.o: ../libc/include/libio.h ../libc/include/stdarg.h
shell.o: ../libc/include/stdlib.h ../libc/include/malloc.h
shell.o: ../libc/include/string.h ../libc/include/time.h
shell.o: include/shell_utils.h include/apps.h ../include/sem.h
shell.o: ../include/debug.h ../libc/include/unistd.h
shell.o: ../kernel/include/module.h ../include/symtable.h
shell.o: ../libc/include/termios.h
shell_utils.o: ../include/types.h ../libc/include/stdio.h
shell_utils.o: ../libc/include/libio.h ../libc/include/sys/types.h
shell_utils.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
shell_utils.o: ../libc/include/malloc.h ../libc/include/string.h
shell_utils.o: include/shell_utils.h
tests.o: ../include/floppy.h ../include/types.h ../include/kmalloc.h
tests.o: ../include/process.h ../include/elf.h ../libc/include/stdio.h
tests.o: ../libc/include/libio.h ../libc/include/sys/types.h
tests.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
tests.o: ../libc/include/malloc.h ../libc/include/string.h
tests.o: ../libc/include/time.h include/apps.h ../include/sem.h
tests.o: ../include/debug.h ../libc/include/unistd.h ../libc/include/ctype.h
tests.o: ../libc/include/errno.h ../include/rtl8139.h ../libc/include/fcntl.h
