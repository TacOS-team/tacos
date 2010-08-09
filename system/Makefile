INCLUDES=-I../include/ -Iinclude/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS=*.c

all: libsystem

libsystem: syscall.o sem.o process.o
	$(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

process.o: ../include/types.h ../include/stdlib.h ../include/stdio.h
process.o: ../include/fcntl.h ../include/libio.h ../include/kfcntl.h
process.o: ../include/stdarg.h ../include/syscall.h ../include/process.h
process.o: ../include/pagination.h ../include/vmm.h
sem.o: ../include/sem.h ../include/types.h ../include/syscall.h
sem.o: ../include/process.h ../include/libio.h ../include/fcntl.h
sem.o: ../include/pagination.h ../include/vmm.h
syscall.o: ../include/syscall.h ../include/types.h
