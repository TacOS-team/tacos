#OBJ=cube top matrix test test_sig demo cat ls fopen snake
OBJ=top test_sig zen cat ls mkdir kill
SUBDIRS=mishell
LIBS=../lib/libc.a ../lib/libsystem.a 
INCLUDES=-I../include/ -I../libc/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
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

demo: demo.o
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

ls: ls.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

fopen: fopen.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

mkdir: mkdir.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

zen: zen.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

snake: snake.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

kill: kill.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

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

cat.o: ../libc/include/stdio.h ../libc/include/libio.h ../include/types.h
cat.o: ../libc/include/stdarg.h
cube.o: ../libc/include/stdio.h ../libc/include/libio.h ../include/types.h
cube.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
cube.o: ../libc/include/malloc.h ../libc/include/time.h ../include/process.h
cube.o: ../include/vmm.h ../include/signal.h ../libc/include/string.h
cube.o: ../libc/include/unistd.h ../libc/include/sys/types.h
demo.o: ../libc/include/stdio.h ../libc/include/libio.h ../include/types.h
demo.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
demo.o: ../libc/include/malloc.h ../libc/include/time.h ../include/process.h
demo.o: ../include/vmm.h ../include/signal.h ../libc/include/string.h
demo.o: ../libc/include/unistd.h ../libc/include/sys/types.h
fiinou.o: ../libc/include/stdio.h ../libc/include/libio.h ../include/types.h
fiinou.o: ../libc/include/stdarg.h
fopen.o: ../libc/include/stdio.h ../libc/include/libio.h ../include/types.h
fopen.o: ../libc/include/stdarg.h
kill.o: ../libc/include/sys/types.h ../include/signal.h ../include/types.h
kill.o: ../libc/include/stdio.h ../libc/include/libio.h
kill.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
kill.o: ../libc/include/malloc.h
ls.o: ../libc/include/dirent.h ../include/types.h ../libc/include/stdio.h
ls.o: ../libc/include/libio.h ../libc/include/stdarg.h
ls.o: ../libc/include/unistd.h ../libc/include/sys/types.h
matrix.o: ../libc/include/stdio.h ../libc/include/libio.h ../include/types.h
matrix.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
matrix.o: ../libc/include/malloc.h ../libc/include/unistd.h
matrix.o: ../libc/include/sys/types.h ../include/process.h ../include/vmm.h
matrix.o: ../include/signal.h
mkdir.o: ../libc/include/dirent.h ../include/types.h ../libc/include/stdio.h
mkdir.o: ../libc/include/libio.h ../libc/include/stdarg.h
sem_ping.o: ../include/sem.h ../include/types.h ../libc/include/stdio.h
sem_ping.o: ../libc/include/libio.h ../libc/include/stdarg.h
sem_ping.o: ../libc/include/unistd.h ../libc/include/sys/types.h
sem_pong.o: ../include/sem.h ../include/types.h ../libc/include/stdio.h
sem_pong.o: ../libc/include/libio.h ../libc/include/stdarg.h
sem_pong.o: ../libc/include/unistd.h ../libc/include/sys/types.h
snake.o: ../include/signal.h ../include/types.h ../libc/include/stdio.h
snake.o: ../libc/include/libio.h ../libc/include/stdarg.h
snake.o: ../libc/include/stdlib.h ../libc/include/malloc.h
snake.o: ../libc/include/time.h ../libc/include/unistd.h
snake.o: ../libc/include/sys/types.h ../libc/include/termios.h
test.o: ../libc/include/stdio.h ../libc/include/libio.h ../include/types.h
test.o: ../libc/include/stdarg.h ../libc/include/unistd.h
test.o: ../libc/include/sys/types.h
test_sig.o: ../libc/include/stdio.h ../libc/include/libio.h
test_sig.o: ../include/types.h ../libc/include/stdarg.h
test_sig.o: ../libc/include/sys/types.h ../include/signal.h
top.o: ../libc/include/stdio.h ../libc/include/libio.h ../include/types.h
top.o: ../libc/include/stdarg.h ../libc/include/time.h ../include/process.h
top.o: ../include/vmm.h ../include/signal.h ../libc/include/unistd.h
top.o: ../libc/include/sys/types.h
zen.o: ../libc/include/stdio.h ../libc/include/libio.h ../include/types.h
zen.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
zen.o: ../libc/include/malloc.h ../libc/include/time.h
zen.o: ../libc/include/string.h ../libc/include/unistd.h
zen.o: ../libc/include/sys/types.h
