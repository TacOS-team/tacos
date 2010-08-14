INCLUDES=-I../include/ -Iinclude/ -I../libc/include/
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

process.o: ../include/types.h ../libc/include/stdlib.h
process.o: ../libc/include/malloc.h ../libc/include/stdio.h
process.o: ../libc/include/fcntl.h ../include/kfcntl.h
process.o: ../libc/include/libio.h ../libc/include/stdarg.h
process.o: ../include/syscall.h ../include/process.h ../include/pagination.h
process.o: ../include/vmm.h
sem.o: ../include/sem.h ../include/types.h ../include/syscall.h
sem.o: ../include/process.h ../libc/include/libio.h ../libc/include/fcntl.h
sem.o: ../include/pagination.h ../include/vmm.h
syscall.o: ../include/syscall.h ../include/types.h
