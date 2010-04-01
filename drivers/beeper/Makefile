INCLUDES=-I../../include/ -Iinclude/
LDFLAGS=-L../../lib/
LDLIBS=-lc

beeper: beeper.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

beeper.c: ../../include/beeper.h ../../include/types.h ../../include/ioports.h ../../include/stdio.h

clean:
	rm -f *.o

