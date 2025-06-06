Program for Statically Allocating Major Number
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

******************************************************************************************************************************************************************

 Flow
 
 1. Introduction
 ---------------------
		We already know what drivers are, and why we need them. What is so special about character drivers? If we write drivers for byte-oriented operations then we refer to them as character drivers. Since the majority of devices are byte-oriented, the majority of device drivers are character device drivers. Take, for example, serial drivers, audio drivers, video drivers, camera drivers, and basic I/O drivers. In fact, all device drivers that are neither storage nor network device drivers are some type of character driver.
 
2. How Applications will communicate with Hardware devices?
----------------------------------------------------------------

					APPlication 
							  <<-- Two way
					Devide File pr Device Node

					Major Minor Number
					 
					Devide Driver 
					 		<<-- Two way
					Hardware
					
#Initially, the Application will open the device file. This device file is created by the device driver).
#Then this device file will find the corresponding device driver using major and minor numbers.
#Then that Device driver will talk to the Hardware device.


3. Character Device Driver Major Number and Minor Number
-----------------------------------------------------------------

	One of the basic features of the Linux kernel is that it abstracts the handling of devices. All hardware devices look like regular files; they can be opened, closed, read, and written using the same, standard, system calls that are used to manipulate files. To Linux, everything is a file. To write to the hard disk, you write to a file. To read from the keyboard is to read from a file. To store backups on a tape device is to write to a file. Even to read from memory is to read from a file. If the file from which you are trying to read or to which you are trying to write is a “normal” file, the process is fairly easy to understand: the file is opened and you read or write data. So the device driver also likes the file. The driver will create a special file for every hardware device. We can communicate with the hardware using those special files (device files).

If you want to create a special file, we should know about the major number and minor number in the device driver. In this tutorial, we will learn the major and minor numbers.


# 3.4 Major Number and Minor Number
----------------------------------------
#3.4.1 Major number
	Traditionally, the major number identifies the driver associated with the device. A major number can also be shared by multiple device drivers. See /proc/devices to find out how major numbers are assigned on a running Linux instance.

In board 
--------------------------
[root@BeagleBone ~]#  cat /proc/devices | grep "Dev"
235 Nil_Dev
[root@BeagleBone ~]# 

#3.4.2 Minor Number
	The major number is to identify the corresponding driver. Many devices may use the same major number. So we need to assign the number to each device which is using the same major number. So, this is a minor number. In other words, The device driver uses the minor number <minor> to distinguish individual physical or logical devices.

Allocating Major and Minor Number
======================================

#1.Statically allocating
#2.Dynamically Allocating




#1.Statically allocating
*****************************
If you want to set a particular major number for your driver, you can use this method. This method will allocate that major number if it is available. Otherwise, it won’t.

#int register_chrdev_region(dev_t first, unsigned int count, char *name);

Here, first is the beginning device number of the range you would like to allocate.

count is the total number of contiguous device numbers you are requesting. Note that, if the count is large, the range you request could spill over to the next major number; but everything will still work properly as long as the number range you request is available.

name is the name of the device that should be associated with this number range; it will appear in /proc/devices and sysfs.


The return value from register_chrdev_region will be 0 if the allocation was successfully performed. In case of error, a negative error code will be returned, and you will not have access to the requested region.

The dev_t type (defined in <linux/types.h>) is used to hold device numbers—both the major and minor parts. dev_t is a 32-bit quantity with 12 bits set aside for the major number and 20 for the minor number.

If you want to create the dev_t structure variable for your major and minor number, please use the below function.

# MKDEV(int major, int minor);

#If you want to get your major number and minor number from dev_t, use the below method.

# MAJOR(dev_t dev);
# MMINOR(dev_t dev);

If you pass the dev_t structure to this MAJOR or MONOR function, it will return that major/minor number of your driver.

//creating the dev with our custom major and minor number
dev_t dev = MKDEV(235, 0);


#dev_t dev = MKDEV(235, 0);
#register_chrdev_region(dev, 1, "Embetronicx_Dev");

#2Dynamically Allocating
*********************************
If we don’t want the fixed major and minor numbers please use this method. This method will allocate the major number dynamically to your driver which is available.

#   int alloc_chrdev_region(dev_t *dev, unsigned int firstminor, unsigned int count, char *name);


#	"dev" is an output-only parameter that will, on successful completion, hold the first number in your allocated range.

#	"firstminor" should be the requested first minor number to use; it is usually 0.


#	"count" is the total number of contiguous device numbers you are requesting.

#	"name" is the name of the device that should be associated with this number range; it will appear in /proc/devices and sysfs.



# 6.3 Difference between static and dynamic method
-----------------------------------------------------
	A static method is only really useful if you know in advance which major number you want to start with. With the Static method, you are telling the kernel that what device numbers you want (the start major/minor number and count) and it either gives them to you or not (depending on availability).

With the Dynamic method, you are telling the kernel that how many device numbers you need (the starting minor number and count) and it will find a starting major number for you, if one is available, of course.

Partially to avoid conflict with other device drivers, it’s considered preferable to use the Dynamic method function, which will dynamically allocate the device numbers for you.

The disadvantage of dynamic assignment is that you can’t create the device nodes in advance, because the major number assigned to your module will vary. For normal use of the driver, this is hardly a problem, because once the number has been assigned, you can read it from /proc/devices.




#Unregister the Major and Minor Number
********************************************
Regardless of how you allocate your device numbers, you should free them when they are no longer in use. Device numbers are freed with:

#void unregister_chrdev_region(dev_t first, unsigned int count);

The usual place to call unregister_chrdev_region would be in your module’s cleanup function (Exit Function).


















































































































































 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 









