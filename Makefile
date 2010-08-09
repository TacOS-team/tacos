export MAKE=make

USECLANG=0

ifneq ($(USECLANG), 1)
	HASCOLOR = $(shell if test `which colorgcc`; then echo true; else echo false; fi)
	ifneq ($(HASCOLOR),true)
		export CC=@printf "\033[34m   CC   $$@\033[0m\n" && gcc
	else
		export CC=@printf "\033[34m   CC   $$@\033[0m\n" && colorgcc
	endif
else
	export CC=@printf "\033[34m   CC   $$@\033[0m\n" && clang
endif

export AR=@printf "\033[32m   AR   $$@\033[0m\n" && ar

export LD=@printf "\033[31m   LD   $$@\033[0m\n" && ld
export CFLAGS=-native -W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
LDLIBS=-lc -lpci -lclock -lutils -ldrivers -z nodefaultlib -lsystem
LDFLAGS=-Llib/
SUBDIRS = kernel libc utils drivers pci clock apps system 

all: kernel.bin
	@readelf --syms kernel.bin | awk '{print $2 " " $8}' > symbols

kernel.bin: force_look
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i; \
		if [ $$? = 0 ]; then printf "\033[1m<<< [$$i] [OK]\033[0m\n"; else printf "\033[31m\033[1m<<< [$$i] [FAIL]\033[0m\n"; exit 1; fi; \
	done
	$(LD) -T linker.ld -o kernel.bin kernel/*.o apps/*.o -melf_i386 $(LDFLAGS) $(LDLIBS)

force_look:
	@true

core.img: img

img: all
	@echo "drive v: file=\"`pwd`/core.img\" 1.44M filter" > mtoolsrc
	@gzip -dc < grub.img.gz > core.img
	MTOOLSRC=mtoolsrc mcopy menu.txt v:/boot/grub/
	MTOOLSRC=mtoolsrc mcopy doc.txt v:/
	MTOOLSRC=mtoolsrc mcopy a.out v:/
	MTOOLSRC=mtoolsrc mmd v:/system
	MTOOLSRC=mtoolsrc mcopy kernel.bin v:/system/
	@rm mtoolsrc

runqemu: core.img
	qemu -fda core.img -soundhw pcspk -m 4 

runqemugdb: core.img
	qemu -fda core.img -soundhw pcspk -parallel none -m 4 -s -S

runbochs: core.img
	BOCHSRC=bochsrc bochs

.PHONY: clean depend doc

doc:
	doxygen doxygen.conf && $(MAKE) -C doc/latex

clean:
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i clean; \
		printf "\033[1m<<< [$$i]\033[0m\n"; \
	done
	@rm -f *.o *.bin *.img
	@rm lib/*

depend:
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i depend; \
		printf "\033[1m<<< [$$i]\033[0m\n"; \
	done
