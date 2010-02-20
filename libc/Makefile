INCLUDES=-I../include/

all: libc

libc: stdio.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	echo $(CFLAGS)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

stdio.o: ../include/stdio.h ../include/ioports.h ../include/types.h

stdlib.o: ../include/stdlib.h ../include/types.h

clean:
	rm -f *.o

mrproper: clean
	rm ../lib/libc.a

