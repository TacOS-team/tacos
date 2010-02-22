INCLUDE=-I../include/ -Iinclude/
LDFLAGS=-L../lib/
LDLIBS=-lc
CFLAGS=-W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
  
all: boot.o exception_wrappers.o interrupts_wrappers.o kernel.o i8259.o idt.o gdt.o memory.o exception.o interrupts.o scheduler.o dummy_process.o process.o 

boot.o: boot.S
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE) -fno-stack-protector

exception_wrappers.o: exception_wrappers.S ../include/exception.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE) -DASM_SOURCE=1 -fno-stack-protector

interrupts_wrappers.o: interrupts_wrappers.S ../include/interrupts.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE) -DASM_SOURCE=1 -fno-stack-protector

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE) 

kernel.o: include/multiboot.h ../include/types.h ../include/memory.h ../include/stdio.h include/gdt.h include/idt.h include/scheduler.h include/dummy_process.h ../include/clock.h

i8259.o: include/i8259.h ../include/types.h

idt.o: include/idt.h ../include/types.h

gdt.o: include/gdt.h ../include/types.h

memory.o: ../include/memory.h ../include/types.h

exception.o: ../include/exception.h ../include/types.h include/idt.h

interrupts.o: ../include/interrupts.h ../include/types.h include/idt.h

scheduler.o: ../include/stdio.h ../include/types.h include/scheduler.h

process.o : ../include/stdio.h ../include/stdlib.h ../include/process.h ../include/types.h

dummy_process.o: ../include/stdio.h ../include/types.h include/dummy_process.h

clean:
	rm -f *.o

