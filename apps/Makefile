INCLUDES=-I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: fiinou.o shell.o shell_utils.o tests.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o
	
depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE

fiinou.o: ../include/stdio.h ../include/libio.h ../include/fcntl.h
fiinou.o: ../include/types.h ../include/stdarg.h
shell.o: include/fat_test.h ../include/types.h ../include/fcntl.h
shell.o: ../include/floppy.h ../include/kmalloc.h ../include/memory.h
shell.o: ../include/mouse.h ../include/pci.h ../include/process.h
shell.o: ../include/pagination.h ../include/libio.h ../include/stdio.h
shell.o: ../include/stdarg.h ../include/stdlib.h ../include/string.h
shell.o: ../include/time.h include/shell_utils.h include/apps.h
shell.o: ../include/sem.h ../include/debug.h
shell_utils.o: ../include/types.h ../include/stdio.h ../include/libio.h
shell_utils.o: ../include/fcntl.h ../include/stdarg.h ../include/stdlib.h
shell_utils.o: ../include/string.h include/shell_utils.h
tests.o: include/fat_test.h ../include/types.h ../include/fcntl.h
tests.o: ../include/floppy.h ../include/kmalloc.h ../include/memory.h
tests.o: ../include/mouse.h ../include/pci.h ../include/process.h
tests.o: ../include/pagination.h ../include/libio.h ../include/stdio.h
tests.o: ../include/stdarg.h ../include/stdlib.h ../include/string.h
tests.o: ../include/time.h include/apps.h ../include/sem.h ../include/debug.h
