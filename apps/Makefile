INCLUDES=-I../include/ -Iinclude/ -I../libc/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: shell.o fiinou.o tests.o shell_utils.o pres.o snake.o noxeyes.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o
	
depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE

fiinou.o: ../libc/include/stdio.h ../libc/include/fcntl.h
fiinou.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
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
shell.o: ../include/fat.h ../include/types.h ../include/kfcntl.h
shell.o: ../libc/include/libio.h ../libc/include/fcntl.h ../include/floppy.h
shell.o: ../include/kmalloc.h ../include/memory.h ../include/mouse.h
shell.o: ../include/pci.h ../include/process.h ../include/pagination.h
shell.o: ../include/vmm.h ../include/signal.h ../libc/include/stdio.h
shell.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
shell.o: ../libc/include/malloc.h ../libc/include/string.h
shell.o: ../libc/include/time.h include/shell_utils.h include/apps.h
shell.o: ../include/sem.h ../include/debug.h ../include/serial.h
shell.o: ../libc/include/unistd.h ../include/clock.h
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
tests.o: ../include/fat.h ../include/types.h ../include/kfcntl.h
tests.o: ../libc/include/libio.h ../libc/include/fcntl.h ../include/floppy.h
tests.o: ../include/kmalloc.h ../include/mouse.h ../include/pci.h
tests.o: ../include/process.h ../include/pagination.h ../include/vmm.h
tests.o: ../include/signal.h ../libc/include/stdio.h ../libc/include/stdarg.h
tests.o: ../libc/include/stdlib.h ../libc/include/malloc.h
tests.o: ../libc/include/string.h ../libc/include/time.h include/apps.h
tests.o: ../include/sem.h ../include/gui.h ../include/memory.h
tests.o: ../include/debug.h ../libc/include/unistd.h ../include/list.h
tests.o: ../include/serial.h ../include/elf.h ../libc/include/ctype.h
