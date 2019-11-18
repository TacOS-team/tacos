OBJ=tests
LIBS=../../lib/libtsock.a ../../lib/libc.a ../../lib/libsystem.a ../../lib/libstl.a ../libutils.a
LDFLAGS=-L./
INCLUDES=-I../../include/ -I../../libs/libc/include/ -I../../system/include/ -I../../libs/tsock/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -fno-builtin -I`pwd` -m32 -fexec-charset=iso-8859-1  -mno-sse -mno-mmx
LD=ld -melf_i386 -Ttext=40000000 --entry=start_main
BIN=../../bin/
SRCS=*.c

all: $(OBJ) 

-include .depend

%.o: %.c
	$(CC) -o $@ -c $< $(INCLUDES) $(CFLAGS)

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $(CPPINCLUDES) $^ > .depend

tests: tests.o test_stdio.o test_malloc.o test_sig.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

clean:
	rm -f *.o $(OBJ)
