INCLUDES=-I../../include/
SRCS = fopen.c  fprintf.c  get.c  printf.c  put.c  scanf.c  sprintf.c  stdfiles.c  stdio.c

stdio: stdio.o scanf.o stdfiles.o fprintf.o printf.o put.o sprintf.o get.o

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	rm -f *.o

depend: $(SRCS)
		makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

fopen.o: ../../include/stdio.h ../../include/libio.h ../../include/fcntl.h
fopen.o: ../../include/types.h ../../include/stdarg.h ../../include/string.h
fprintf.o: ../../include/stdio.h ../../include/libio.h ../../include/fcntl.h
fprintf.o: ../../include/types.h ../../include/stdarg.h
fprintf.o: ../../include/string.h
get.o: ../../include/stdio.h ../../include/libio.h ../../include/fcntl.h
get.o: ../../include/types.h ../../include/stdarg.h
printf.o: ../../include/stdio.h ../../include/libio.h ../../include/fcntl.h
printf.o: ../../include/types.h ../../include/stdarg.h
put.o: ../../include/stdio.h ../../include/libio.h ../../include/fcntl.h
put.o: ../../include/types.h ../../include/stdarg.h ../../include/stdlib.h
put.o: ../../include/unistd.h
scanf.o: ../../include/ctype.h ../../include/stdarg.h ../../include/stdlib.h
scanf.o: ../../include/types.h ../../include/stdio.h ../../include/libio.h
scanf.o: ../../include/fcntl.h ../../include/string.h
sprintf.o: ../../include/stdio.h ../../include/libio.h ../../include/fcntl.h
sprintf.o: ../../include/types.h ../../include/stdarg.h
stdfiles.o: ../../include/types.h ../../include/libio.h ../../include/fcntl.h
stdfiles.o: ../../include/string.h ../../include/kmalloc.h
stdio.o: ../../include/types.h ../../include/ioports.h ../../include/ctype.h
