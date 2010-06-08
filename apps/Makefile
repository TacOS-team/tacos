INCLUDES=-I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: shell.o fiinou.o matrix.o tests.o shell_utils.o pres.o snake.o noxeyes.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o
	
depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE

fiinou.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
fiinou.o: ../include/libio.h ../include/stdarg.h
matrix.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
matrix.o: ../include/libio.h ../include/stdarg.h ../include/stdlib.h
matrix.o: ../include/unistd.h
noxeyes.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
noxeyes.o: ../include/libio.h ../include/stdarg.h ../include/stdlib.h
noxeyes.o: ../include/video.h ../include/process.h ../include/pagination.h
noxeyes.o: ../include/mouse.h
pres.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
pres.o: ../include/libio.h ../include/stdarg.h ../include/gui.h
pres.o: ../include/process.h ../include/pagination.h
shell.o: ../include/fat.h ../include/types.h ../include/fcntl.h
shell.o: ../include/libio.h ../include/floppy.h ../include/kmalloc.h
shell.o: ../include/memory.h ../include/mouse.h ../include/pci.h
shell.o: ../include/process.h ../include/pagination.h ../include/stdio.h
shell.o: ../include/stdarg.h ../include/stdlib.h ../include/string.h
shell.o: ../include/time.h include/shell_utils.h include/apps.h
shell.o: ../include/sem.h ../include/video.h ../include/debug.h
shell_utils.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
shell_utils.o: ../include/libio.h ../include/stdarg.h ../include/stdlib.h
shell_utils.o: ../include/string.h include/shell_utils.h
snake.o: ../include/stdio.h ../include/fcntl.h ../include/types.h
snake.o: ../include/libio.h ../include/stdarg.h ../include/stdlib.h
snake.o: ../include/time.h ../include/process.h ../include/pagination.h
snake.o: ../include/video.h
tests.o: ../include/fat.h ../include/types.h ../include/fcntl.h
tests.o: ../include/libio.h ../include/floppy.h ../include/kmalloc.h
tests.o: ../include/mouse.h ../include/pci.h ../include/process.h
tests.o: ../include/pagination.h ../include/stdio.h ../include/stdarg.h
tests.o: ../include/stdlib.h ../include/string.h ../include/time.h
tests.o: include/apps.h ../include/sem.h ../include/gui.h ../include/memory.h
tests.o: ../include/debug.h
