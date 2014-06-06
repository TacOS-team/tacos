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
	export CC=@printf "\033[34m   CC   $$@\033[0m\n" && clang -no-integrated-as
endif

export CPPC=@printf "\033[34m   CPPC   $$@\033[0m\n" && g++

ifeq ($(WERROR), 1)
	export WE="-Werror"
endif


export AR=@printf "\033[32m   AR   $$@\033[0m\n" && ar

export LD=@printf "\033[31m   LD   $$@\033[0m\n" && ld
export CFLAGS=-native $(WE) -W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32
LDLIBS=-lc -ldrivers -z nodefaultlib -lsystem -lstl
LDLIBSKERNEL=-ldrivers -z nodefaultlib -lsystem
LDFLAGS=-Llib/
SUBDIRS = kernel kernel/drivers libc system libs/stl libs/tsock applications

all: directories kernel.bin

kernel.bin: force_look
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i; \
		if [ $$? = 0 ]; then printf "\033[1m<<< [$$i] [OK]\033[0m\n"; else printf "\033[31m\033[1m<<< [$$i] [FAIL]\033[0m\n"; exit 1; fi; \
	done
	$(LD) -T linker.ld -o kernel.bin kernel/*.o kernel/klibc/*.o kernel/fs/*.o kernel/fs/fat/*.o kernel/fs/ext2/*.o kernel/pci/*.o kernel/utils/*.o -melf_i386 $(LDFLAGS) $(LDLIBSKERNEL)

force_look:
	@true
	
test:
	$(MAKE) -s -C tests;

core.img: all
#	@echo "drive v: file=\"`pwd`/core.img\" 1.44M filter" > mtoolsrc
	@dd if=/dev/zero of=core.img bs=512 count=2880
	@mkfs.ext2 -F core.img
	@e2cp README core.img:/
	@e2cp script.sh core.img:/
	@e2mkdir core.img:/bin
	@e2cp -p bin/* core.img:/bin/
#	@mkfs.vfat core.img
#	@MTOOLSRC=mtoolsrc mcopy README v:/
#	@MTOOLSRC=mtoolsrc mcopy bin v:/
	
grub.img: all
	@echo "drive v: file=\"`pwd`/grub.img\" 1.44M filter" > mtoolsrc
	@gzip -dc < grub.img.gz > grub.img
	@MTOOLSRC=mtoolsrc mcopy menu.txt v:/boot/grub/
	@MTOOLSRC=mtoolsrc mmd v:/system
	@MTOOLSRC=mtoolsrc mcopy kernel.bin v:/system/
	@rm mtoolsrc

runqemunet: core.img grub.img
	qemu -fda grub.img -fdb core.img -vga std -net nic,model=rtl8139,macaddr=AC:DC:DE:AD:BE:EF -net tap,ifname=tap0,script=no -net dump,file=eth.log -m 20 

runqemu: core.img grub.img 
	qemu -fda grub.img -drive file=core.img,index=1,if=floppy,cache=writeback -vga std -parallel none -m 20 -serial stdio -serial vc -serial vc -serial vc
	
runqemugdb: core.img grub.img
	qemu -fda grub.img -fdb core.img -vga std -parallel none -m 20 -s -S -serial stdio -serial vc -serial vc -serial vc
	
runbochs: core.img grub.img
	BOCHSRC=bochsrc bochs

.PHONY: clean depend doc directories

directories:
	@mkdir -p lib
	@mkdir -p bin
	@mkdir -p modules

doc:
	doxygen doxygen.conf

dockernel:
	doxygen doxygen_kernel.conf

doclibc:
	doxygen doxygen_libc.conf

docpdf: doc
	$(MAKE) -C doc/latex

clean:
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i clean; \
		$(MAKE) -s -C $$i depend; \
		printf "\033[1m<<< [$$i]\033[0m\n"; \
	done
	@rm -f *.o *.bin *.img
	@rm -f lib/*
	@rm -f bin/*
	@find . -name "*.o" -exec rm {} \;

depend:
	@for i in $(SUBDIRS); do \
		printf "\033[1m>>> [$$i]\033[0m\n"; \
		$(MAKE) -s -C $$i depend; \
		printf "\033[1m<<< [$$i]\033[0m\n"; \
	done

installhooks:
	cp -f git/hooks/* .git/hooks/
