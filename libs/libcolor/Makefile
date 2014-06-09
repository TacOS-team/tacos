INCLUDES = -I../../include/ -I../../libc/include/ -I../stl/ -I../tsock/ -I.
CXXFLAGS = -W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -fpermissive -fno-rtti -fno-exceptions
SRCS = *.cpp

all: libcolor

libcolor: color.o
	$(AR) r ../../lib/$@.a $^

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES)

clean:
	rm -f *.o .depend

mrproper: clean
	@rm ../../lib/libcolor.a

-include .depend

depend: $(SRCS)
	  $(CC) -MM $(CXXFLAGS) $(INCLUDES) $^ > .depend
