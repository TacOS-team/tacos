CPPINCLUDES=-I../../include/ -I../../libc/include/ -I.
CPPFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -fno-rtti -fno-exceptions

SRCS=*.cpp

all: libstl

libstl: stl_string.o stl_stdlib.o
	$(AR) r ../../lib/$@.a $^

%.o: %.cpp
	$(CPPC) -o $@ -c $< $(CPPFLAGS) $(CPPINCLUDES) 

clean:
	@rm -f *.o

depend: $(SRCS)
	makedepend -Y $(CPPINCLUDES) -- $(CPPFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

stl_stdlib.o: ./stl_stdlib.h ../../libc/include/malloc.h
stl_stdlib.o: ../../libc/include/sys/types.h ../../include/types.h
stl_string.o: ./stl_string.h ../../include/types.h ./stdlib ./stl_stdlib.h
stl_string.o: ../../libc/include/malloc.h ../../libc/include/sys/types.h
stl_string.o: ../../libc/include/cstring ../../libc/include/string.h
