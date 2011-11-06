CPPINCLUDES=-I../../include/ -I../../libc/include/
CPPFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -fno-rtti -fno-exceptions

SRCS=*.cpp

all: libstl

libstl: stl_string.o
	$(AR) r ../../lib/$@.a $^

%.o: %.cpp
	$(CPPC) -o $@ -c $< $(CPPFLAGS) $(CPPINCLUDES) 

clean:
	@rm -f *.o

depend: $(SRCS)
	makedepend -Y $(CPPINCLUDES) -- $(CPPFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it
