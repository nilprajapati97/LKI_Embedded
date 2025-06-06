obj-m += driver.o
 
ifdef ARCH
	#You can update your Beaglebone path here.
        KDIR = /home/infinite/LKI/bbb-builds/OS/linux-4.19.103/
else
	KDIR = /lib/modules/$(shell uname -r)/build
endif
 
 
all:
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean
