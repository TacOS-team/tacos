INCLUDES=-I../include/ -Iinclude/ -I../libc/include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: libclock

libclock: clock.o events.o i8254.o
	$(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o
	
depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

clock.o: ../include/types.h ../include/ioports.h include/i8254.h
clock.o: ../include/clock.h ../libc/include/time.h ../include/interrupts.h
clock.o: ../include/heap.h
events.o: ../include/types.h include/i8254.h ../include/interrupts.h
events.o: ../libc/include/time.h ../include/clock.h ../include/list.h
events.o: ../include/events.h ../libc/include/stdio.h ../libc/include/fcntl.h
events.o: ../libc/include/libio.h ../libc/include/stdarg.h ../include/debug.h
i8254.o: include/i8254.h ../include/types.h ../include/ioports.h
