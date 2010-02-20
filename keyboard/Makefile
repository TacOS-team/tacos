INCLUDES=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc

all: libkeyboard

libkeyboard: keyboard.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

keyboard.c: ../include/keyboard.h ../include/types.h ../include/ioports.h ../include/stdio.h

clean:
	rm -f *.o

