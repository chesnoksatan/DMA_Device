obj-m += dma.o
TARGET = dma

CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/kernel/$(MDIR)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
	-rm -f *.o *.ko .*.cmd .*.flags *.mod.c

install: all
	sudo insmod $(TARGET).ko

unload:
	sudo rmmod $(TARGET).ko
	sudo dmesg -c
