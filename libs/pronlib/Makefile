INCLUDES = -I../../include/ -I../../libc/include/ -I../libcolor/ -I../stl/ -I../tsock/ -I.
CXXFLAGS = -W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -fpermissive -fno-rtti -fno-exceptions
SRCS = *.cpp

all: pronlib

pronlib: pronlib.o
	$(AR) r ../../lib/$@.a $^

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES)

clean:
	rm -f *.o .depend

mrproper: clean
	@rm ../../lib/pronlib.a

-include .depend

depend: $(SRCS)
	  $(CC) -MM $(CXXFLAGS) $(INCLUDES) $^ > .depend
