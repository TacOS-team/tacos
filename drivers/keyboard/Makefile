INCLUDES=-I../../include/ -Iinclude/
LDFLAGS=-L../../lib/
LDLIBS=-lc

SRCS=keyboard.c

keyboard: keyboard.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o

depend: $(SRCS)
		makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

keyboard.o: ../../include/types.h ../../include/stdio.h ../../include/libio.h
keyboard.o: ../../include/fcntl.h ../../include/stdarg.h
keyboard.o: ../../include/stdlib.h ../../include/ioports.h
keyboard.o: ../../include/keyboard.h ../../include/process.h
keyboard.o: ../../include/pagination.h
