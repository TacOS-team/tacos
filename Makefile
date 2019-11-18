export MAKE=make

USECLANG=0
WERROR=0

QEMU=qemu-system-i386

ifneq ($(USECLANG), 1)
	export CC=@printf "\033[34m   CC   $$@\033[0m\n" && gcc -fdiagnostics-color
else
	export CC=@printf "\033[34m   CC   $$@\033[0m\n" && clang -no-integrated-as
endif

export CPPC=@printf "\033[34m   CPPC   $$@\033[0m\n" && g++

ifeq ($(WERROR), 1)
	export WE="-Werror"
endif


export AR=@printf "\033[32m   AR   $$@\033[0m\n" && ar

export LD=@printf "\033[31m   LD   $$@\033[0m\n" && ld
export CFLAGS=-native $(WE) -W -Wall -g -nostdlib -nostdinc -nostartfiles -nodefaultlibs -fno-builtin -I`pwd` -m32  -fexec-charset=iso-8859-1  -mno-sse -mno-mmx
LDLIBS=-lc -ldrivers -z nodefaultlib -ltacos -lstl
LDLIBSKERNEL=-ldrivers -z nodefaultlib -ltacos
LDFLAGS=-Llib/
SUBDIRS = kernel kernel/drivers libs/libc libs/stl libs/tsock libs/tacos applications
#SUBDIRS = kernel kernel/drivers libs/libc libs/stl libs/tsock libs/tacos libs/pronlib libs/libcolor libs/libjpeg applications

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
#	@strip bin/*
	@e2cp -p bin/* core.img:/bin/
# Uncomment the 3 next lines to use pr0n.
#	@e2mkdir core.img:/bin/fonts
#	@e2cp -p applications/gui/pron/ressources/fonts/* core.img:/bin/fonts/
#	@e2cp applications/gui/tacos.jpg core.img:/bin/
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
	$(QEMU) -drive file=grub.img,format=raw,index=0,if=floppy -drive file=core.img,format=raw,index=1,if=floppy,cache=writeback -vga std -net nic,model=rtl8139,macaddr=AC:DC:DE:AD:BE:EF -net tap,ifname=tap0,script=no -net dump,file=eth.log -m 256 

runqemu: core.img grub.img 
	$(QEMU) -drive file=grub.img,format=raw,index=0,if=floppy -drive file=core.img,format=raw,index=1,if=floppy,cache=writeback -vga std -parallel none -m 256 -serial stdio -serial vc -serial vc -serial vc
	
runqemugdb: core.img grub.img
	$(QEMU) -drive file=grub.img,format=raw,index=0,if=floppy -drive file=core.img,format=raw,index=1,if=floppy,cache=writeback -vga std -parallel none -m 256 -s -S -serial stdio -serial vc -serial vc -serial vc
	
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
