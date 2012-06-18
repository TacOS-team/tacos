LIBS=../../lib/libc.a ../../lib/libsystem.a
INCLUDES=-I../../include/ -I../../libc/include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -D_BSD_SOURCE
CC=gcc $(INCLUDES) 
LD=ld -melf_i386 -Ttext=40000000 --entry=start_main
BIN=../../bin/
EXEC_MANE = bulot
SRC = $(wildcard *.c)
OBJ =  lex.yy.o y.tab.o $(SRC:.c=.o)

all: $(EXEC_MANE) 

lex.yy.c: interpreter.lex y.tab.c
	flex interpreter.lex

y.tab.c: interpreter.yacc
	yacc -dv interpreter.yacc

bulot: $(OBJ)
	$(LD) -o $(BIN)$@ $^ $(LIBS)

.o: .c 
	$(CC) c $^ $(CFLAGS)

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

clean:
	@rm -f *.o
	@rm -f lex.yy.c y.tab.c y.tab.h y.output
	@rm -f $(BIN)./bulot
