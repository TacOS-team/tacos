INCLUDES=-I../include/

all: libc

libc: stdio/stdio.o stdlib.o string.o unistd.o time.o ctype.o errno.o stdio/sscanf.o stdio/stdfiles.o stdio/fprintf.o stdio/printf.o stdio/put.o stdio/sprintf.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

stdlib.o: ../include/stdlib.h ../include/types.h ../include/ctype.h

string.o: ../include/string.h ../include/types.h

unistd.o: ../include/unistd.h ../include/types.h

time.o :  ../include/time.h ../include/types.h

errno.o :  ../include/errno.h

ctype.o: ../include/ctype.h

stdio: force_look
	$(MAKE) -C $@ 

clean:
	$(MAKE) -C stdio clean
	rm -f *.o

mrproper: clean
	rm ../lib/libc.a

force_look:
	@true

