OBJ=mishell
LIBS=../../lib/libc.a ../../lib/libsystem.a
INCLUDES=-I../../include/ -I../../libc/include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
CC=gcc $(INCLUDES) 
LD=ld -melf_i386 -Ttext=40000000 --entry=start_main
BIN=../../bin/
SRCS=*.c

all: $(OBJ) 

mishell: mishell.o shell_utils.o readline.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

.o: .c 
	$(CC) c $^ $(CFLAGS)

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

clean:
	rm -f $(OBJ) *.o *.a
	rm -f $(BIN)*

# DO NOT DELETE
