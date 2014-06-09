OBJS = pron.o gc.o screen.o mouse.o keyboard.o window.o client.o clip_zone.o clip_rect.o drawable.o windowsTree.o pixmap.o font.o font_loader.o
LIBS = ../../../lib/libtsock.a ../../../lib/libcolor.a ../../../lib/libstl.a ../../../lib/libtacos.a ../../../lib/libc.a ../../../lib/libsystem.a
INCLUDES = -I../../../include/ -I ../../../libs/tsock/ -I../../../libs/pronlib/ -I../../../libs/libcolor/ -I../../../libs/tacos/ -I../../../libc/include/ -I../../../libs/stl/ -I.
CXXFLAGS = -W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -fpermissive -fno-rtti -fno-exceptions -O3
LD = ld -melf_i386 -Ttext=40000000 --entry=start_main
BIN = ../../../bin
SRCS = *.cpp

all: $(BIN)/pron

$(BIN)/pron: $(OBJS)
	$(LD) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(INCLUDES)
	
.PHONY: clean
