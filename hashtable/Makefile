
MODNAME=hasht
obj-m += $(MODNAME).o
$(MODNAME)-objs := kern/$(MODNAME).o


KERN_SRC=/lib/modules/$(shell uname -r)/build

all: kernel-module-uninstall kernel-clean-ring-buffer kernel-build kernel-clean-temporary kernel-module-install 
	@tput setaf 3
	@echo "    done: all"
	@tput sgr0
clean: kernel-module-uninstall kernel-clean user-clean
	@tput setaf 3
	@echo "    done: clean"
	@tput sgr0
 
 
 
kernel-build:
	@tput setaf 1
	@echo "    kernel-build"
	@tput sgr0
	make -C $(KERN_SRC) M=$(PWD) modules  
kernel-clean:
	@tput setaf 1
	@echo "    kernel-clean"
	@tput sgr0
	make -C $(KERN_SRC) M=$(PWD) clean
kernel-clean-temporary:
	@tput setaf 1
	@echo "    kernel-clean-temporary"
	@tput sgr0
	-rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions
	-rm -rf kern/*.o kern/*~ kern/core kern/.depend kern/.*.cmd kern/*.mod.c kern/.tmp_versions
	-rm -rf Module.symvers modules.order
kernel-module-install:
	@tput setaf 1
	@echo "    kernel-module-install"
	@tput sgr0
	-sudo insmod $(MODNAME).ko
kernel-module-uninstall:
	@tput setaf 1
	@echo "    kernel-module-uninstall"
	@tput sgr0
	-sudo rmmod $(MODNAME)
kernel-clean-ring-buffer:
	@tput setaf 1
	@echo "    kernel-clean-ring-buffer"
	@tput sgr0
	sudo dmesg -c > /dev/null

