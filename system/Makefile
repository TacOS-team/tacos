INCLUDES=-I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS=*.c

all: libsystem

libsystem: syscall.o sem.o
	$(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

sem.o: ../include/sem.h ../include/types.h ../include/syscall.h
sem.o: ../include/process.h ../include/pagination.h ../include/libio.h
sem.o: ../include/fcntl.h
syscall.o: ../include/syscall.h ../include/types.h
