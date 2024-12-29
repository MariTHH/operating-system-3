KERNELDIR ?= /usr/src/linux-headers-$(shell uname -r)
PWD := $(shell pwd)
MODULE := netstat_procfs

obj-m += $(MODULE).o

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
