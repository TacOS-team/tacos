#OBJ=cube top matrix demo cat ls fopen snake
OBJ=getty ps ls cat ln rm mv top kill demo snake cube_graph cube date
SUBDIRS=mishell utils tests
#OBJ=ls getty
#SUBDIRS=mishell utils gui gui/guacamole gui/pron
LIBS=../lib/libtsock.a ../lib/libc.a ../lib/libstl.a libutils.a
LDFLAGS=-L./
INCLUDES=-I../libs/tacos/include/ -I../libs/libc/include/ -I../libs/tsock/
CPPINCLUDES=-I../libs/stl/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -fno-builtin -I`pwd` -m32 -mno-sse -mno-mmx
CPPFLAGS=-W -Wall -g -nostdlib -nostdinc -fno-builtin -I`pwd` -m32 -fno-rtti -fno-exceptions
LD=ld -Ttext=40000000 --entry=start_main -melf_i386
BIN=../bin/
SRCS=*.c *.cpp

all: subdirs $(OBJ)

subdirs:
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i; \
		if [ $$? = 0 ]; then printf "\033[1m<<< [$$i] [OK]\033[0m\n"; else printf "\033[31m\033[1m<<< [$$i] [FAIL]\033[0m\n"; exit 1; fi; \
		done

-include .depend

depend: $(SRCS)
	$(CC) -MM $(CFLAGS) $(INCLUDES) $(CPPINCLUDES) $^ > .depend

%: %.o subdirs
	$(LD) -o $(BIN)$@ $< $(LIBS)

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

