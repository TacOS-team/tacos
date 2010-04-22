INCLUDES=-I../../include/

stdio: stdio.o scanf.o stdfiles.o fprintf.o printf.o put.o sprintf.o get.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

stdio.o: ../../include/stdio.h ../../include/ioports.h ../../include/types.h ../../include/libio.h

scanf.o : ../../include/stdio.h ../../include/ctype.h ../../include/types.h ../../include/stdarg.h

stdfiles.o : ../../include/types.h ../../include/libio.h

fprintf.o: ../../include/stdio.h

printf.o: ../../include/stdio.h

sprintf.o: ../../include/stdio.h

scanf.o: ../../include/stdio.h

put.o: ../../include/stdio.h

get.o: ../../include/stdio.h

clean:
	rm -f *.o
