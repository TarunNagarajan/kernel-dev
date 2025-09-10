
obj-m += process-list.o

KVER := 6.8.0-79-generic
KDIR := /usr/src/linux-headers-$(KVER)

EXTRA_CFLAGS += -DMODULE_VERMAGIC_STRING='"$(KVER) SMP preempt mod_unload modversions"'

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
