MODULE_NAME	:= test
ifneq ($(KERNELRELEASE),)
$(MODULE_NAME)-objs=../../src/list.o
$(MODULE_NAME)-objs+=../../src/log.o
$(MODULE_NAME)-objs+=../../modules/kmodule.o
obj-m:=$(MODULE_NAME).o
else
KERNEL_DIR=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)
all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD)
insert:
	dmesg -c
	insmod ./$(MODULE_NAME).ko
remove:
	rmmod $(MODULE_NAME).ko
clean:
	rm -rf ../../src/*.o ./*.o *.mod *.ko* modules.order Module.symvers *.pc.in
endif

