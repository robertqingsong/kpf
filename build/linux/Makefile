ifneq ($(KERNELRELEASE),)
kpf-objs=../../src/list.o
kpf-objs+=../../src/log.o
obj-m:=kpf.o
else
WARN	:= -W -Wall -Wstrict-prototypes -Wmissing-prototypes
INCLUDE	:= -I/media/workbench/dev/kpf/kpf/inc
CFLAGS	:= -DMODULE -D__KERNEL__ ${WARN} $(INCLUDE)
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

