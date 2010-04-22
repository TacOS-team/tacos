INCLUDES=-I../include/ -I../kernel/include/

all: libc

libc: stdio/stdio.o stdlib.o string.o unistd.o time.o ctype.o errno.o stdio/scanf.o stdio/stdfiles.o stdio/fprintf.o stdio/printf.o stdio/put.o stdio/sprintf.o stdio/get.o libio.o fcntl.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

stdlib.o: ../include/stdlib.h ../include/types.h ../include/ctype.h

string.o: ../include/string.h ../include/types.h

unistd.o: ../include/unistd.h ../include/types.h

time.o :  ../include/time.h ../include/types.h

errno.o :  ../include/errno.h

ctype.o: ../include/ctype.h

libio.o: ../include/libio.h

fcntl.o: ../include/types.h ../include/fcntl.h ../include/process.h ../include/stdio.h ../kernel/include/fat.h

stdio: force_look
	$(MAKE) -C $@ 

clean:
	$(MAKE) -C stdio clean
	rm -f *.o

mrproper: clean
	rm ../lib/libc.a

force_look:
	@true

