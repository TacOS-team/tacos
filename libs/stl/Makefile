CPPINCLUDES=-I../../include/ -I../../libc/include/ -I.
CPPFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -fno-rtti -fno-exceptions

SRCS=*.cpp

all: libstl

libstl: stl_string.o stl_stdlib.o icxxabi.o
	$(AR) r ../../lib/$@.a $^

%.o: %.cpp
	$(CPPC) -o $@ -c $< $(CPPFLAGS) $(CPPINCLUDES) 

clean:
	@rm -f *.o .depend

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CPPFLAGS) $(CPPINCLUDES) $^ > .depend
