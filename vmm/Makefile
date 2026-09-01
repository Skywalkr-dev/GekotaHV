obj-m += ghv.o

GHV_SRC := $(wildcard src/*.c)
ghv-objs := $(GHV_SRC:.c=.o)

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean