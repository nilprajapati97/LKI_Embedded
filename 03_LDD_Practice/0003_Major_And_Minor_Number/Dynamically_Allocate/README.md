Program for Dynamically Allocating Major Number
===============================================================================

1. Build the driver by using Makefile (sudo make) or if you are using the Beaglebone board,
   then you can use sudo make "ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-."
   
2. Load the driver using sudo insmod
3. Check the major number using cat /proc/devices


/*
********************************************  OUTPUT   ******************************************************************
*/   

[root@BeagleBone ~]# cat /proc/devices | grep "Nil_Dev"
235 Nil_Dev


[root@BeagleBone ~]# cat /proc/devices 
Character devices:
  1 mem
  4 /dev/vc/0
  4 tty
  4 ttyS
  5 /dev/tty
  5 /dev/console
  5 /dev/ptmx
  7 vcs
 10 misc
 13 input
 29 fb
 89 i2c
 90 mtd
116 alsa
128 ptm
136 pts
180 usb
189 usb_device
226 drm
235 Nil_Dev
241 gnss
242 hidraw
243 rpmb
244 ttyGS
245 aux
246 bsg
247 watchdog
248 iio
249 ptp
250 pps
251 cec
252 rtc
253 tpm
254 gpiochip

Block devices:
  1 ramdisk
  7 loop
  8 sd
 43 nbd
 65 sd
 66 sd
 67 sd
 68 sd
 69 sd
 70 sd
 71 sd
128 sd
129 sd
130 sd
131 sd
132 sd
133 sd
134 sd
135 sd
179 mmc
254 ubiblock
259 blkext
[root@BeagleBone ~]# 
