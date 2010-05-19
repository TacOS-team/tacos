INCLUDES=-I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: fiinou.o shell.o shell_utils.o tests.o pres.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o
	
depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE

fiinou.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
fiinou.o: ../include/libio.h ../include/stdarg.h
pres.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
pres.o: ../include/libio.h ../include/stdarg.h ../include/gui.h
pres.o: ../include/process.h
shell.o: include/fat_test.h ../include/types.h ../include/fcntl.h
shell.o: ../include/libio.h ../include/floppy.h ../include/kmalloc.h
shell.o: ../include/memory.h ../include/mouse.h ../include/pci.h
shell.o: ../include/process.h ../include/stdio.h ../include/stdarg.h
shell.o: ../include/stdlib.h ../include/string.h ../include/time.h
shell.o: include/shell_utils.h include/apps.h ../include/sem.h
shell.o: ../include/debug.h
shell_utils.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
shell_utils.o: ../include/libio.h ../include/stdarg.h ../include/stdlib.h
shell_utils.o: ../include/string.h include/shell_utils.h
tests.o: include/fat_test.h ../include/types.h ../include/fcntl.h
tests.o: ../include/libio.h ../include/floppy.h ../include/kmalloc.h
tests.o: ../include/memory.h ../include/mouse.h ../include/pci.h
tests.o: ../include/process.h ../include/stdio.h ../include/stdarg.h
tests.o: ../include/stdlib.h ../include/string.h ../include/time.h
tests.o: include/apps.h ../include/sem.h ../include/gui.h ../include/debug.h
