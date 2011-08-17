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
noxeyes.o: ../libc/include/malloc.h ../include/process.h
noxeyes.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
noxeyes.o: ../include/mouse.h
pres.o: ../libc/include/stdio.h ../libc/include/fcntl.h
pres.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
pres.o: ../include/gui.h ../include/process.h ../include/pagination.h
pres.o: ../include/vmm.h ../include/signal.h
shell.o: ../libc/include/dirent.h ../include/floppy.h ../include/types.h
shell.o: ../include/kmalloc.h ../include/memory.h ../include/mouse.h
shell.o: ../include/pci.h ../include/pci_types.h ../include/process.h
shell.o: ../libc/include/libio.h ../libc/include/fcntl.h
shell.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
shell.o: ../libc/include/stdio.h ../libc/include/stdarg.h
shell.o: ../libc/include/stdlib.h ../libc/include/malloc.h
shell.o: ../libc/include/string.h ../libc/include/time.h
shell.o: include/shell_utils.h include/apps.h ../include/sem.h
shell.o: ../include/debug.h ../libc/include/unistd.h
shell.o: ../kernel/include/module.h ../include/symtable.h
shell_utils.o: ../include/types.h ../libc/include/stdio.h
shell_utils.o: ../libc/include/fcntl.h ../libc/include/libio.h
shell_utils.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
shell_utils.o: ../libc/include/malloc.h ../libc/include/string.h
shell_utils.o: include/shell_utils.h
snake.o: ../libc/include/stdio.h ../libc/include/fcntl.h
snake.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
snake.o: ../libc/include/stdlib.h ../libc/include/malloc.h
snake.o: ../libc/include/time.h ../include/process.h ../include/pagination.h
snake.o: ../include/vmm.h ../include/signal.h
tests.o: ../include/fat.h ../include/floppy.h ../include/types.h
tests.o: ../include/kmalloc.h ../include/process.h ../libc/include/libio.h
tests.o: ../libc/include/fcntl.h ../include/pagination.h ../include/vmm.h
tests.o: ../include/signal.h ../libc/include/stdio.h ../libc/include/stdarg.h
tests.o: ../libc/include/stdlib.h ../libc/include/malloc.h
tests.o: ../libc/include/string.h ../libc/include/time.h include/apps.h
tests.o: ../include/sem.h ../include/memory.h ../include/debug.h
tests.o: ../libc/include/unistd.h ../include/elf.h ../libc/include/ctype.h
tests.o: ../include/errno.h ../include/rtl8139.h
