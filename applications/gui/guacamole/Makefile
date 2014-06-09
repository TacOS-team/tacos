OBJS = guacamole.o gwindows_manager.o gwindow.o
LIBS = ../../../lib/pronlib.a ../../../lib/libcolor.a ../../../lib/libtsock.a ../../../lib/libstl.a ../../../lib/libtacos.a ../../../lib/libc.a ../../../lib/libsystem.a
INCLUDES = -I../../../include/ -I../../../libs/pronlib/ -I../../../libs/libcolor/ -I../../../libs/tacos/ -I../../../libc/include/ -I../../../libs/stl/ -I../../../libs/tsock/ -I.
CXXFLAGS = -W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -fpermissive -fno-rtti -fno-exceptions
LD = ld -melf_i386 -Ttext=40000000 --entry=start_main
BIN = ../../../bin
SRCS = *.cpp

all: $(BIN)/guacamole

$(BIN)/guacamole: $(OBJS)
	$(LD) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES)
	
.PHONY: clean
