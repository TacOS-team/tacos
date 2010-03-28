INCLUDES=-I../include/

all: libc

libc: stdio.o stdlib.o string.o unistd.o time.o ctype.o errno.o sscanf.o stdfiles.o fprintf.o printf.o put.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

stdio.o: ../include/stdio.h ../include/ioports.h ../include/types.h ../include/libio.h

stdlib.o: ../include/stdlib.h ../include/types.h ../include/ctype.h

string.o: ../include/string.h ../include/types.h

unistd.o: ../include/unistd.h ../include/types.h

time.o :  ../include/time.h ../include/types.h

errno.o :  ../include/errno.h

sscanf.o : ../include/stdio.h ../include/ctype.h ../include/types.h ../include/stdarg.h

stdfiles.o : ../include/types.h ../include/libio.h

ctype.o: ../include/ctype.h

fprintf.o: ../include/stdio.h

printf.o: ../include/stdio.h

put.o: ../include/stdio.h

clean:
	rm -f *.o

mrproper: clean
	rm ../lib/libc.a
