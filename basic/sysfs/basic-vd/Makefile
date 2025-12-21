obj-m += basic-vd.o

WSL2_KERNEL := $(HOME)/WSL2-Linux-Kernel
KDIR := $(WSL2_KERNEL)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
