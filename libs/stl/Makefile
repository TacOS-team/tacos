CPPINCLUDES=-I../../include/ -I../../libc/include/
CPPFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -fno-rtti -fno-exceptions

SRCS=*.cpp

all: libstl

libstl: stl_vector.o
	$(AR) r ../../lib/$@.a $^

%.o: %.cpp
	$(CPPC) -o $@ -c $< $(CPPFLAGS) $(CPPINCLUDES) 

clean:
	@rm -f *.o

depend: $(SRCS)
	makedepend -Y $(CPPINCLUDES) -- $(CPPFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

stl_vector.o: /home/barbug/Sources/INSA/tacos/libs/stl/stl_vector.h
stl_vector.o: ../../libc/include/cstdio ../../libc/include/stdio.h
stl_vector.o: ../../libc/include/libio.h ../../libc/include/sys/types.h
stl_vector.o: ../../libc/include/stdarg.h
