This is just a basic linux device driver. This will explain how to pass the arguments to the linux device driver.

Please update your Beaglebone board's kernel directory in the Makefile.

Build for Beaglebone: sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-

Build for Raspberry Pi or Virtualbox Ubuntu: sudo make

Run Steps -:

1 -: sudo insmod hello_world.ko valueETX=14 nameETX="EmbeTronicX" arr_valueETX=100,102,104,106
2 -: dmesg
3 -: sudo rmmod hello_world.ko
4 -: dmesg

