#OBJ=cube top matrix test test_sig demo cat ls fopen snake
OBJ=cat ls kill snake zen_graph 
SUBDIRS=mishell
LIBS=../lib/libc.a ../lib/libsystem.a ../lib/libutils.a  ../lib/libstl.a
INCLUDES=-I../include/ -I../libc/include/
CPPINCLUDES=-I../libs/stl/
CFLAGS=-W -O3 -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
CPPFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32 -fno-rtti -fno-exceptions
LD=ld -Ttext=40000000 --entry=start_main -melf_i386
BIN=../bin/
SRCS=*.c *.cpp

all: $(OBJ)
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i; \
		if [ $$? = 0 ]; then printf "\033[1m<<< [$$i] [OK]\033[0m\n"; else printf "\033[31m\033[1m<<< [$$i] [FAIL]\033[0m\n"; exit 1; fi; \
		done

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $(CPPINCLUDES) $^ > .depend

%: %.o
	$(LD) -o $(BIN)$@ $^ $(LIBS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

%.o: %.cpp
	$(CPPC) -o $@ -c $< $(CPPFLAGS) $(INCLUDES) $(CPPINCLUDES)

clean:
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i clean; \
		printf "\033[1m<<< [$$i]\033[0m\n"; \
	done
	rm -f $(OBJ) *.o *.a .depend
	rm -f $(BIN)*

