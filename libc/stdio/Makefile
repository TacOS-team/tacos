INCLUDES=-I../../include/

stdio: stdio.o sscanf.o stdfiles.o fprintf.o printf.o put.o sprintf.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

stdio.o: ../../include/stdio.h ../../include/ioports.h ../../include/types.h ../../include/libio.h

sscanf.o : ../../include/stdio.h ../../include/ctype.h ../../include/types.h ../../include/stdarg.h

stdfiles.o : ../../include/types.h ../../include/libio.h

fprintf.o: ../../include/stdio.h

printf.o: ../../include/stdio.h

sprintf.o: ../../include/stdio.h

put.o: ../../include/stdio.h

clean:
	rm -f *.o
