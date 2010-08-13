INCLUDES=-I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: shell.o fiinou.o matrix.o tests.o shell_utils.o pres.o snake.o demo_cube.o noxeyes.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o
	
depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE

demo_cube.o: ../include/stdio.h ../include/fcntl.h ../include/kfcntl.h
demo_cube.o: ../include/types.h ../include/libio.h ../include/stdarg.h
demo_cube.o: ../include/stdlib.h ../include/time.h ../include/process.h
demo_cube.o: ../include/pagination.h ../include/vmm.h ../include/string.h
fiinou.o: ../include/stdio.h ../include/fcntl.h ../include/kfcntl.h
fiinou.o: ../include/types.h ../include/libio.h ../include/stdarg.h
matrix.o: ../include/stdio.h ../include/fcntl.h ../include/kfcntl.h
matrix.o: ../include/types.h ../include/libio.h ../include/stdarg.h
matrix.o: ../include/stdlib.h ../include/unistd.h ../include/process.h
matrix.o: ../include/pagination.h ../include/vmm.h
noxeyes.o: ../include/stdio.h ../include/fcntl.h ../include/kfcntl.h
noxeyes.o: ../include/types.h ../include/libio.h ../include/stdarg.h
noxeyes.o: ../include/stdlib.h ../include/process.h ../include/pagination.h
noxeyes.o: ../include/vmm.h ../include/mouse.h
pres.o: ../include/stdio.h ../include/fcntl.h ../include/kfcntl.h
pres.o: ../include/types.h ../include/libio.h ../include/stdarg.h
pres.o: ../include/gui.h ../include/process.h ../include/pagination.h
pres.o: ../include/vmm.h
shell.o: ../include/fat.h ../include/types.h ../include/kfcntl.h
shell.o: ../include/libio.h ../include/fcntl.h ../include/floppy.h
shell.o: ../include/kmalloc.h ../include/memory.h ../include/mouse.h
shell.o: ../include/pci.h ../include/process.h ../include/pagination.h
shell.o: ../include/vmm.h ../include/stdio.h ../include/stdarg.h
shell.o: ../include/stdlib.h ../include/string.h ../include/time.h
shell.o: include/shell_utils.h include/apps.h ../include/sem.h
shell.o: ../include/debug.h ../include/serial.h ../include/unistd.h
shell_utils.o: ../include/types.h ../include/stdio.h ../include/fcntl.h
shell_utils.o: ../include/kfcntl.h ../include/libio.h ../include/stdarg.h
shell_utils.o: ../include/stdlib.h ../include/string.h include/shell_utils.h
snake.o: ../include/stdio.h ../include/fcntl.h ../include/kfcntl.h
snake.o: ../include/types.h ../include/libio.h ../include/stdarg.h
snake.o: ../include/stdlib.h ../include/time.h ../include/process.h
snake.o: ../include/pagination.h ../include/vmm.h
tests.o: ../include/fat.h ../include/types.h ../include/kfcntl.h
tests.o: ../include/libio.h ../include/fcntl.h ../include/floppy.h
tests.o: ../include/kmalloc.h ../include/mouse.h ../include/pci.h
tests.o: ../include/process.h ../include/pagination.h ../include/vmm.h
tests.o: ../include/stdio.h ../include/stdarg.h ../include/stdlib.h
tests.o: ../include/string.h ../include/time.h include/apps.h
tests.o: ../include/sem.h ../include/gui.h ../include/memory.h
tests.o: ../include/debug.h ../include/unistd.h ../include/list.h
tests.o: ../include/serial.h ../include/elf.h ../include/ctype.h
