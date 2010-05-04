INCLUDES=-I../../include/ -Iinclude/
LDFLAGS=-L../../lib/
LDLIBS=-lc

SRCS = beeper.c

beeper: beeper.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o

depend: $(SRCS)
		makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

beeper.o: ../../include/beeper.h ../../include/types.h
beeper.o: ../../include/ioports.h
