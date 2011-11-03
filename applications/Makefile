#OBJ=cube top matrix test test_sig demo cat ls fopen snake
OBJ=test_sig cat ls kill clock_graph test_mouse pres zen_graph
SUBDIRS=mishell
LIBS=../lib/libc.a ../lib/libsystem.a ../lib/libutils.a 
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

clock_graph: clock_graph.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

cube_graph: cube_graph.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

test_mouse: test_mouse.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

zen_graph: zen_graph.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)	

pres: pres.o
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

cat.o: ../libc/include/stdio.h ../libc/include/libio.h
cat.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
clock_graph.o: ../libc/include/string.h ../libc/include/sys/types.h
clock_graph.o: ../libc/include/sys/syscall.h ../libc/include/time.h
clock_graph.o: ../libc/include/unistd.h ../include/vga_types.h
cube.o: ../libc/include/stdio.h ../libc/include/libio.h
cube.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
cube.o: ../libc/include/stdlib.h ../libc/include/malloc.h
cube.o: ../libc/include/time.h ../libc/include/string.h
cube.o: ../libc/include/unistd.h
cube_graph.o: ../libc/include/string.h ../libc/include/sys/types.h
cube_graph.o: ../libc/include/sys/syscall.h ../libc/include/unistd.h
cube_graph.o: ../include/vga_types.h
demo.o: ../libc/include/stdio.h ../libc/include/libio.h
demo.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
demo.o: ../libc/include/stdlib.h ../libc/include/malloc.h
demo.o: ../libc/include/time.h ../libc/include/string.h
demo.o: ../libc/include/unistd.h
fiinou.o: ../libc/include/stdio.h ../libc/include/libio.h
fiinou.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
fopen.o: ../libc/include/stdio.h ../libc/include/libio.h
fopen.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
hello.o: ../libc/include/stdio.h ../libc/include/libio.h
hello.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
kill.o: ../libc/include/sys/types.h ../libc/include/signal.h
kill.o: ../libc/include/stdio.h ../libc/include/libio.h
kill.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
kill.o: ../libc/include/malloc.h
ls.o: ../libc/include/dirent.h ../libc/include/sys/types.h
ls.o: ../libc/include/sys/stat.h ../libc/include/stdio.h
ls.o: ../libc/include/libio.h ../libc/include/stdarg.h
ls.o: ../libc/include/unistd.h
matrix.o: ../libc/include/stdio.h ../libc/include/libio.h
matrix.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
matrix.o: ../libc/include/stdlib.h ../libc/include/malloc.h
matrix.o: ../libc/include/unistd.h
mkdir.o: ../libc/include/dirent.h ../libc/include/sys/types.h
mkdir.o: ../libc/include/sys/stat.h ../libc/include/stdio.h
mkdir.o: ../libc/include/libio.h ../libc/include/stdarg.h
pres.o: ../libc/include/stdio.h ../libc/include/libio.h
pres.o: ../libc/include/sys/types.h ../libc/include/stdarg.h ../include/gui.h
pres.o: ../include/types.h
sem_ping.o: ../include/sem.h ../include/types.h ../libc/include/stdio.h
sem_ping.o: ../libc/include/libio.h ../libc/include/sys/types.h
sem_ping.o: ../libc/include/stdarg.h ../libc/include/unistd.h
sem_pong.o: ../include/sem.h ../include/types.h ../libc/include/stdio.h
sem_pong.o: ../libc/include/libio.h ../libc/include/sys/types.h
sem_pong.o: ../libc/include/stdarg.h ../libc/include/unistd.h
snake.o: ../libc/include/signal.h ../libc/include/sys/types.h
snake.o: ../libc/include/stdio.h ../libc/include/libio.h
snake.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
snake.o: ../libc/include/malloc.h ../libc/include/time.h
snake.o: ../libc/include/unistd.h ../libc/include/termios.h
test.o: ../libc/include/stdio.h ../libc/include/libio.h
test.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
test.o: ../libc/include/unistd.h
test_mouse.o: ../libc/include/fcntl.h ../libc/include/stdio.h
test_mouse.o: ../libc/include/libio.h ../libc/include/sys/types.h
test_mouse.o: ../libc/include/stdarg.h ../libc/include/sys/syscall.h
test_mouse.o: ../libc/include/unistd.h ../include/vga_types.h
test_sig.o: ../libc/include/stdio.h ../libc/include/libio.h
test_sig.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
test_sig.o: ../libc/include/signal.h
top.o: ../include/process.h ../libc/include/libio.h
top.o: ../libc/include/sys/types.h ../include/types.h ../include/vmm.h
top.o: ../libc/include/signal.h ../libc/include/stdio.h
top.o: ../libc/include/stdarg.h ../libc/include/stdlib.h
top.o: ../libc/include/malloc.h ../libc/include/time.h
top.o: ../libc/include/unistd.h
zen.o: ../libc/include/stdio.h ../libc/include/libio.h
zen.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
zen.o: ../libc/include/stdlib.h ../libc/include/malloc.h
zen.o: ../libc/include/time.h ../libc/include/string.h
zen.o: ../libc/include/unistd.h
zen_graph.o: ../libc/include/stdio.h ../libc/include/libio.h
zen_graph.o: ../libc/include/sys/types.h ../libc/include/stdarg.h
zen_graph.o: ../libc/include/stdlib.h ../libc/include/malloc.h
zen_graph.o: ../libc/include/time.h ../libc/include/string.h
zen_graph.o: ../libc/include/unistd.h ../include/vga_types.h
zen_graph.o: ../libc/include/sys/syscall.h
