OBJ=mishell
LIBS=../../lib/libc.a ../../lib/libsystem.a
INCLUDES=-I../../include/ -I../../libc/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
LD=ld -melf_i386 -Ttext=40000000 --entry=start_main
BIN=../../bin/
SRCS=*.c

all: $(OBJ) 

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $(CPPINCLUDES) $^ > .depend

mishell: mishell.o shell_utils.o readline.o history.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

%.o: %.c
	$(CC) -o $@ -c $< $(INCLUDES) $(CFLAGS)

clean:
	rm -f $(OBJ) *.o *.a
	rm -f $(BIN)*

