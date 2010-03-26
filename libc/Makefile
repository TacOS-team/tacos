INCLUDES=-I../include/

all: libc

libc: stdio.o stdlib.o string.o unistd.o time.o ctype.o sscanf.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

stdio.o: ../include/stdio.h ../include/ioports.h ../include/types.h

stdlib.o: ../include/stdlib.h ../include/types.h

string.o: ../include/string.h ../include/types.h

unistd.o: ../include/unistd.h ../include/types.h

time.o :  ../include/time.h ../include/types.h

ctype.o : ../include/ctype.h

sscanf.o : ../include/stdio.h ../include/ctype.h ../include/types.h ../include/stdarg.h

clean:
	rm -f *.o

mrproper: clean
	rm ../lib/libc.a
