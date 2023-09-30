
obj-m += gpioExampleWithUART.o


KDIR = /lib/modules/$(shell uname -r)/build

all: module dt

module:
	make -C $(KDIR) M=$(shell pwd) modules
dt: serdevOverlay.dts
	dtc -@ -I dts -O dtb -o serdevOverlay.dtbo serdevOverlay.dts
clean:
	make -C $(KDIR) M=$(shell pwd) clean
	rm -rf serdevOverlay.dtbo
