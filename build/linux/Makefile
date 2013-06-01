MODULE_NAME	:= test
ifneq ($(KERNELRELEASE),)
$(MODULE_NAME)-objs=../../src/list.o
$(MODULE_NAME)-objs+=../../src/log.o
obj-m:=$(MODULE_NAME).o
else
KERNEL_DIR=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)
all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD)
insert:
	insmod ./kpf.ko
remove:
	rmmod kpf.ko
clean:
	rm -rf *.o *.mod *.ko* modules.order Module.symvers *.pc.in
endif

