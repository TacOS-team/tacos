INCLUDES=-I. -I../include/
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32

SRCS = *.c

all: libpci

libpci: pci.o pci_config.o
	$(AR) r ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

clean:
	@rm -f *.o

depend: $(SRCS)
	makedepend -Y $(INCLUDES) -- $(CFLAGS) -- $^

# DO NOT DELETE THIS LINE -- make depend needs it

pci.o: ../include/pci.h ../include/types.h ../include/pci_config.h
pci.o: ../include/pci_types.h ../include/stdio.h ../include/libio.h
pci.o: ../include/fcntl.h ../include/stdarg.h
pci_config.o: ../include/ioports.h ../include/types.h ../include/pci_config.h
pci_config.o: ../include/pci_types.h ./pci_vendor.h ../include/stdio.h
pci_config.o: ../include/libio.h ../include/fcntl.h ../include/stdarg.h
