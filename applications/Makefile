OBJ=cube top matrix test test_sig fiinou cat
SUBDIRS=mishell
LIBS=../lib/libc.a ../lib/libsystem.a 
INCLUDES=-I../include/ -I../libc/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
CC=gcc $(INCLUDES)
LD=ld -Ttext=40000000 --entry=start_main -melf_i386
BIN=../bin/
SRCS=*.c

all: $(OBJ)
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i; \
		if [ $$? = 0 ]; then printf "\033[1m<<< [$$i] [OK]\033[0m\n"; else printf "\033[31m\033[1m<<< [$$i] [FAIL]\033[0m\n"; exit 1; fi; \
		done

fiinou: fiinou.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

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
	
cat: cat.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

.o: .c 
	$(CC) -c $^ $(CFLAGS)

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

clean:
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i clean; \
		printf "\033[1m<<< [$$i]\033[0m\n"; \
	done
	rm -f $(OBJ) *.o *.a
	rm -f $(BIN)*

# DO NOT DELETE

cube.o: ../libc/include/stdio.h ../libc/include/fcntl.h
cube.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
cube.o: ../libc/include/stdlib.h ../libc/include/malloc.h
cube.o: ../libc/include/time.h ../include/process.h ../include/pagination.h
cube.o: ../include/vmm.h ../include/signal.h ../libc/include/string.h
cube.o: ../libc/include/unistd.h
matrix.o: ../libc/include/stdio.h ../libc/include/fcntl.h
matrix.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
matrix.o: ../libc/include/stdlib.h ../libc/include/malloc.h
matrix.o: ../libc/include/unistd.h ../include/process.h
matrix.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
sem_ping.o: ../include/sem.h ../include/types.h ../libc/include/stdio.h
sem_ping.o: ../libc/include/fcntl.h ../libc/include/libio.h
sem_ping.o: ../libc/include/stdarg.h
sem_pong.o: ../include/sem.h ../include/types.h ../libc/include/stdio.h
sem_pong.o: ../libc/include/fcntl.h ../libc/include/libio.h
sem_pong.o: ../libc/include/stdarg.h
test.o: ../libc/include/stdio.h ../libc/include/fcntl.h
test.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
test.o: ../libc/include/stdlib.h ../libc/include/malloc.h
test.o: ../include/process.h ../include/pagination.h ../include/vmm.h
test.o: ../include/signal.h
test_sig.o: ../libc/include/stdio.h ../libc/include/fcntl.h
test_sig.o: ../libc/include/libio.h ../include/types.h
test_sig.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
test_sig.o: ../libc/include/malloc.h ../include/process.h
test_sig.o: ../include/pagination.h ../include/vmm.h ../include/signal.h
top.o: ../libc/include/stdio.h ../libc/include/fcntl.h
top.o: ../libc/include/libio.h ../include/types.h ../libc/include/stdarg.h
top.o: ../libc/include/time.h ../include/process.h ../include/pagination.h
top.o: ../include/vmm.h ../include/signal.h ../libc/include/unistd.h
