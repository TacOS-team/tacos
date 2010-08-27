OBJ=cube top matrix
LIBS=../lib/libc.a ../lib/libsystem.a 
INCLUDES=-I../include/ -I../libc/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
CC=gcc $(INCLUDES) 
LD=ld -Ttext=40000000 --entry=start_main
BIN=../bin/
SRCS=*.c

all: $(OBJ) 

cube: cube.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)
	
top: top.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)
	
matrix: matrix.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)


.o: .c 
	$(CC) c $^ $(CFLAGS)

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

clean:
	rm -f $(OBJ) *.o *.a
	rm -f $(BIN)*

# DO NOT DELETE

cube.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
cube.o: ../include/types.h ../libc/include/libio.h ../libc/include/stdarg.h
cube.o: ../libc/include/stdlib.h ../libc/include/malloc.h
cube.o: ../libc/include/time.h ../include/process.h ../include/pagination.h
cube.o: ../include/vmm.h ../libc/include/string.h
matrix.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
matrix.o: ../include/types.h ../libc/include/libio.h ../libc/include/stdarg.h
matrix.o: ../libc/include/stdlib.h ../libc/include/malloc.h
matrix.o: ../libc/include/unistd.h ../include/process.h
matrix.o: ../include/pagination.h ../include/vmm.h
top.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
top.o: ../include/types.h ../libc/include/libio.h ../libc/include/stdarg.h
top.o: ../libc/include/time.h ../include/process.h ../include/pagination.h
top.o: ../include/vmm.h
