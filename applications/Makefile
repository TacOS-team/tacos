OBJ=cube top matrix test sem_ping sem_pong test_sig
LIBS=../lib/libc.a ../lib/libsystem.a 
INCLUDES=-I../include/ -I../libc/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
CC=gcc $(INCLUDES)
LD=ld -Ttext=40000000 --entry=start_main -melf_i386
BIN=../bin/
SRCS=*.c

all: $(OBJ)
	make -C mishell

cube: cube.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)
	
top: top.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)
	
matrix: matrix.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

test: test.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)
	
sem_ping: sem_ping.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

sem_pong: sem_pong.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

test_sig: test_sig.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

.o: .c 
	$(CC) -c $^ $(CFLAGS)

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
cube.o: ../include/vmm.h ../include/signal.h ../libc/include/string.h
matrix.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
matrix.o: ../include/types.h ../libc/include/libio.h ../libc/include/stdarg.h
matrix.o: ../libc/include/stdlib.h ../libc/include/malloc.h
matrix.o: ../libc/include/unistd.h ../include/process.h
matrix.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
sem_ping.o: ../include/sem.h ../include/types.h ../libc/include/stdio.h
sem_ping.o: ../libc/include/fcntl.h ../include/kfcntl.h
sem_ping.o: ../libc/include/libio.h ../libc/include/stdarg.h
sem_pong.o: ../include/sem.h ../include/types.h ../libc/include/stdio.h
sem_pong.o: ../libc/include/fcntl.h ../include/kfcntl.h
sem_pong.o: ../libc/include/libio.h ../libc/include/stdarg.h
test.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
test.o: ../include/types.h ../libc/include/libio.h ../libc/include/stdarg.h
test.o: ../libc/include/stdlib.h ../libc/include/malloc.h
test.o: ../include/process.h ../include/pagination.h ../include/vmm.h
test.o: ../include/signal.h
test_sig.o: ../libc/include/stdio.h ../libc/include/fcntl.h
test_sig.o: ../include/kfcntl.h ../include/types.h ../libc/include/libio.h
test_sig.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
test_sig.o: ../libc/include/malloc.h ../include/process.h
test_sig.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
top.o: ../libc/include/stdio.h ../libc/include/fcntl.h ../include/kfcntl.h
top.o: ../include/types.h ../libc/include/libio.h ../libc/include/stdarg.h
top.o: ../libc/include/time.h ../include/process.h ../include/pagination.h
top.o: ../include/vmm.h ../include/signal.h ../libc/include/unistd.h
