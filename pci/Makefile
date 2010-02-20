INCLUDES=-I. -I../include/
LDFLAGS=-L../lib/
LDLIBS=-lc

all: libpci

libpci: pci.o pci_config.o
	  ar rv ../lib/$@.a $^

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES) 

pci.o: ../include/pci.h ../include/pci_config.h ../include/pci_types.h ../include/types.h

pci_config.o: ../include/pci_config.h ../include/pci_types.h pci_vendor.h

clean:
	rm -f *.o

