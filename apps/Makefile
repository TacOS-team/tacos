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
noxeyes.o: ../include/process.h ../include/types.h ../include/vmm.h
noxeyes.o: ../libc/include/signal.h
pres.o: ../libc/include/stdio.h ../libc/include/libio.h
pres.o: ../libc/include/sys/types.h ../libc/include/stdarg.h ../include/gui.h
pres.o: ../include/types.h ../include/process.h ../include/vmm.h
pres.o: ../libc/include/signal.h
shell.o: ../libc/include/dirent.h ../libc/include/sys/types.h
shell.o: ../libc/include/sys/stat.h ../include/floppy.h ../include/types.h
shell.o: ../include/kmalloc.h ../include/pci.h ../include/pci_types.h
shell.o: ../include/process.h ../libc/include/libio.h ../include/vmm.h
shell.o: ../libc/include/signal.h ../libc/include/stdio.h
shell.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
shell.o: ../libc/include/malloc.h ../libc/include/string.h
shell.o: ../libc/include/time.h include/shell_utils.h include/apps.h
shell.o: ../include/sem.h ../include/debug.h ../libc/include/unistd.h
shell.o: ../kernel/include/module.h ../include/symtable.h
shell.o: ../libc/include/termios.h
shell_utils.o: ../include/types.h ../libc/include/stdio.h
shell_utils.o: ../libc/include/libio.h ../libc/include/sys/types.h
shell_utils.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
shell_utils.o: ../libc/include/malloc.h ../libc/include/string.h
shell_utils.o: include/shell_utils.h
tests.o: ../include/floppy.h ../include/types.h ../include/kmalloc.h
tests.o: ../include/process.h ../libc/include/libio.h
tests.o: ../libc/include/sys/types.h ../include/vmm.h
tests.o: ../libc/include/signal.h ../libc/include/stdio.h
tests.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
tests.o: ../libc/include/malloc.h ../libc/include/string.h
tests.o: ../libc/include/time.h include/apps.h ../include/sem.h
tests.o: ../include/debug.h ../libc/include/unistd.h ../include/elf.h
tests.o: ../libc/include/ctype.h ../libc/include/errno.h ../include/rtl8139.h
tests.o: ../libc/include/fcntl.h
