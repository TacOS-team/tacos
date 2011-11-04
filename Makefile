export MAKE=make

USECLANG=0
WERROR=0

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

ifeq ($(WERROR), 1)
	export WE="-Werror"
endif


export AR=@printf "\033[32m   AR   $$@\033[0m\n" && ar

export LD=@printf "\033[31m   LD   $$@\033[0m\n" && ld
export CFLAGS=-native $(WE) -W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
LDLIBS=-lc -lutils -ldrivers -z nodefaultlib -lsystem
LDFLAGS=-Llib/
SUBDIRS = kernel libc utils drivers system applications

all: directories kernel.bin

kernel.bin: force_look
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i; \
		if [ $$? = 0 ]; then printf "\033[1m<<< [$$i] [OK]\033[0m\n"; else printf "\033[31m\033[1m<<< [$$i] [FAIL]\033[0m\n"; exit 1; fi; \
	done
	$(LD) -T linker.ld -o kernel.bin kernel/*.o kernel/drivers/*.o apps/*.o -melf_i386 $(LDFLAGS) $(LDLIBS)

force_look:
	@true
core.img: img

img: all
	@echo "drive v: file=\"`pwd`/core.img\" 1.44M filter" > mtoolsrc
	@gzip -dc < grub.img.gz > core.img
	@MTOOLSRC=mtoolsrc mcopy menu.txt v:/boot/grub/
	@MTOOLSRC=mtoolsrc mcopy doc.txt v:/
	@MTOOLSRC=mtoolsrc mcopy bin v:/
	@MTOOLSRC=mtoolsrc mcopy modules v:/
	@MTOOLSRC=mtoolsrc mmd v:/system
	@MTOOLSRC=mtoolsrc mcopy kernel.bin v:/system/
	@rm mtoolsrc

runqemunet: core.img
	qemu -fda core.img -soundhw pcspk -net nic,model=rtl8139,macaddr=AC:DC:DE:AD:BE:EF -net tap,ifname=tap0,script=no -net dump,file=eth.log -m 20 

runqemu: core.img
	qemu -fda core.img -soundhw pcspk -net nic,model=rtl8139,macaddr=AC:DC:DE:AD:BE:EF -net dump,file=eth.log -parallel none -m 20 
runqemugdb: core.img
	qemu -fda core.img -soundhw pcspk -net nic,model=rtl8139,macaddr=AC:DC:DE:AD:BE:EF -net dump,file=eth.log -parallel none -m 20 -s -S
	
runqemucurses: core.img
	qemu -fda core.img -curses -soundhw pcspk -net nic,model=rtl8139,macaddr=AC:DC:DE:AD:BE:EF -net dump,file=eth.log -m 20 

runbochs: core.img
	BOCHSRC=bochsrc bochs

.PHONY: clean depend doc directories

directories:
	@mkdir -p lib
	@mkdir -p bin
	@mkdir -p modules

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

installhooks:
	cp -f git/hooks/* .git/hooks/
