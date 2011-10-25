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

noxeyes.o: ../libc/include/stdio.h ../libc/include/fcntl.h
noxeyes.o: ../libc/include/libio.h ../include/types.h
noxeyes.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
noxeyes.o: ../libc/include/malloc.h ../include/process.h ../include/vmm.h
noxeyes.o: ../include/signal.h ../include/mouse.h
pres.o: ../libc/include/stdio.h ../libc/include/fcntl.h
pres.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
pres.o: ../include/gui.h ../include/process.h ../include/vmm.h
pres.o: ../include/signal.h
shell.o: ../libc/include/dirent.h ../include/types.h ../include/floppy.h
shell.o: ../include/kmalloc.h ../include/mouse.h ../include/pci.h
shell.o: ../include/pci_types.h ../include/process.h ../libc/include/libio.h
shell.o: ../libc/include/fcntl.h ../include/vmm.h ../include/signal.h
shell.o: ../libc/include/stdio.h ../libc/include/stdarg.h
shell.o: ../libc/include/stdlib.h ../libc/include/malloc.h
shell.o: ../libc/include/string.h ../libc/include/time.h
shell.o: include/shell_utils.h include/apps.h ../include/sem.h
shell.o: ../include/debug.h ../libc/include/unistd.h
shell.o: ../libc/include/sys/types.h ../kernel/include/module.h
shell.o: ../include/symtable.h ../libc/include/termios.h
shell_utils.o: ../include/types.h ../libc/include/stdio.h
shell_utils.o: ../libc/include/fcntl.h ../libc/include/libio.h
shell_utils.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
shell_utils.o: ../libc/include/malloc.h ../libc/include/string.h
shell_utils.o: include/shell_utils.h
tests.o: ../include/floppy.h ../include/types.h ../include/kmalloc.h
tests.o: ../include/process.h ../libc/include/libio.h ../libc/include/fcntl.h
tests.o: ../include/vmm.h ../include/signal.h ../libc/include/stdio.h
tests.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
tests.o: ../libc/include/malloc.h ../libc/include/string.h
tests.o: ../libc/include/time.h include/apps.h ../include/sem.h
tests.o: ../include/debug.h ../libc/include/unistd.h
tests.o: ../libc/include/sys/types.h ../include/elf.h ../libc/include/ctype.h
tests.o: ../include/errno.h ../include/rtl8139.h
