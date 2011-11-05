CPPINCLUDES=-I../../include/ -I../../libc/include/
CPPFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -fno-rtti -fno-exceptions

SRCS=*.cpp

all: libstl

libstl: string.o
	$(AR) r ../../lib/$@.a $^

%.o: %.cpp
	$(CPPC) -o $@ -c $< $(CPPFLAGS) $(CPPINCLUDES) 

clean:
	@rm -f *.o

depend: $(SRCS)
	makedepend -Y $(CPPINCLUDES) -- $(CPPFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

stl_vector.o: /home/barbug/Sources/INSA/tacos/libs/stl/stl_vector.h
stl_vector.o: ../../include/types.h ../../libc/include/cstdio
stl_vector.o: ../../libc/include/stdio.h ../../libc/include/libio.h
stl_vector.o: ../../libc/include/sys/types.h ../../libc/include/stdarg.h
