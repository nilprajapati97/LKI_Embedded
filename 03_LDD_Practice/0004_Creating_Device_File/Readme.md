#4.Device Driver 4 - Creating Device File
==============================================


#1 Prerequisites
#2 Device File Creation for Character Drivers
#3 Device Files
#4 Creating Device File
#4.1 Manually Creating Device File
#	4.1.1 Advantages
#	4.1.2 Programming
#	4.2 Automatically Creating Device File
#	4.2.1 Create the class
#	4.2.2 Create Device
#	4.2.3 Programming

----------------------------------------------------------

#1 Prerequisites
	Setup Beaglebone black
	
#2 Device File Creation for Character Drivers
-----------------------------------------------------------

	we have seen how to assign a major and minor number. But if you see, it will create a major and minor number. But I did not create any device files under /dev/ directory. The device file is important to communicate with the hardware. Let’s start our tutorial.
	
#3 Device Files
--------------------------
	The device file allows transparent communication between user-space applications and hardware.

They are not normal “files”, but look like files from the program’s point of view: you can read from them, write to them, mmap() onto them, and so forth. When you access such a device “file,” the kernel recognizes the I/O request and passes it to a device driver, which performs some operation, such as reading data from a serial port or sending data to hardware.

Device files (although inappropriately named, we will continue to use this term) provide a convenient way to access system resources without requiring the application programmer to know how the underlying device works. Under Linux, as with most Unix systems, device drivers themselves are part of the kernel.

All device files are stored in /dev directory. Use ls command to browse the directory:

#	ls -l /dev/

Each device on the system should have a corresponding entry in /dev. For example, /dev/ttyS0 corresponds to the first serial port, known as COM1 under MS-DOS; /dev/hda2 corresponds to the second partition on the first IDE drive. In fact, there should be entries in /dev for devices you do not have. The device files are generally created during system installation and include every possible device driver. They don’t necessarily correspond to the actual hardware on your system. There are a number of pseudo-devices in /dev that don’t correspond to any actual peripheral. For example, /dev/null acts as a byte sink; any write request to /dev/null will succeed, but the data are written will be ignored.

#		When using ls -l to list device files in /dev, you’ll see something like the following:


			infinite@annu:~/Project/Capgemini/Training/LDD_Training/LKI/LKI_BBB/0004_Creating_Device_File$ ls -l /dev
			total 0
			crw-r--r--   1 root     root     10,   235 Jan  8 12:56 autofs
			drwxr-xr-x   2 root     root           680 Jan  8 12:56 block
			crw-------   1 root     root     10,   234 Jan  8 12:56 btrfs-control
			drwxr-xr-x   3 root     root            60 Jan  8 12:56 bus
			drwxr-xr-x   2 root     root          5020 Jan  8 12:56 char
			crw--w----   1 root     tty       5,     1 Jan  8 12:56 console
			lrwxrwxrwx   1 root     root            11 Jan  8 12:56 core -> /proc/kcore
			drwxr-xr-x  14 root     root           300 Jan  8 12:56 cpu
			crw-------   1 root     root     10,   123 Jan  8 12:56 cpu_dma_latency
			crw-------   1 root     root     10,   203 Jan  8 12:56 cuse
			drwxr-xr-x   8 root     root           160 Jan  8 12:56 disk
			
First of all, note that the first letter of the permissions field is denoted that driver type. Device files are denoted either by b, for block devices, or c, for character devices.

Also, note that the size field in the ls -l listing is replaced by two numbers, separated by a comma. The first value is the major device number and the second is the minor device number. This we have discussed in the previous tutorial.

#4 Creating Device File
-------------------------------------------
We can create a dive file in two ways.

*	Manually
*	Automatically
#4.1 Manually Creating Device File
	We can create the device file manually by using mknod.

#		mknod -m <permissions> <name> <device type> <major> <minor>

#		<name> – your device file name that should have a full path (/dev/name)

#		<device type> – Put c or b

#		c – Character Device

#		b – Block Device

#		<major> – major number of your driver

#		<minor> – minor number of your driver

#		-m <permissions> – optional argument that sets the permission bits of the new device file to permissions

#for Examples
#		sudo mknod -m 666 /dev/Infinite_device c 246 0

		If you don’t want to give permission, You can also use chmod to set the permissions for a device file after creation.

#	4.1.1 Advantages
-----------------------------
#	Anyone can create the device file using this method.
#	You can create the device file even before loading the driver.

#	4.1.2 Programming
--------------------------------------------
********************************************************************************************************************************************
		/***************************************************************************//**
		*  \file       driver.c
		*
		*  \details    Simple linux driver (Manually Creating a Device file)
		*
		*  \author     NIL PRAJAPATI
		*
		*  \Tested with Linux BBB linux-4.19.103
		*
		*******************************************************************************/
		#include<linux/kernel.h>
		#include<linux/init.h>
		#include<linux/module.h>
		#include <linux/kdev_t.h>
		#include <linux/fs.h>
		 
		dev_t dev = 0;

		/*
		** Module init function
		*/
		static int __init hello_world_init(void)
		{
			/*Allocating Major number*/
			if((alloc_chrdev_region(&dev, 0, 1, "Infinite_Dev")) <0){
				pr_err("Cannot allocate major number for device\n");
				return -1;
			}
			pr_info("Kernel Module Inserted Successfully...\n");
			return 0;
		}

		/*
		** Module exit function
		*/
		static void __exit hello_world_exit(void)
		{
			unregister_chrdev_region(dev, 1);
			pr_info("Kernel Module Removed Successfully...\n");
		}
		 
		module_init(hello_world_init);
		module_exit(hello_world_exit);
		 
		MODULE_LICENSE("GPL");
		MODULE_AUTHOR(" Infinite < ni.praajapati97@yahoo.com >");
		MODULE_DESCRIPTION("Simple linux driver (Manually Creating a Device file)");
		MODULE_VERSION("1.1");
**************************************************************************************************************
#	Buidd  	Build the driver by using Makefile (sudo make)
--------------------------------------------------------------------
#	Build the driver by using Makefile (sudo make)
#	Load the driver using sudo insmod
#	Check the device file using ls -l /dev/. By this time device file is not created for your driver.
#	Create a device file using mknod and then check using ls -l /dev/.
#	Now our device file got created and registered with a major number and minor number.
#	Unload the driver using sudo rmmod	

#[root@BeagleBone ~]# mknod -m 666 /dev/Infinite_Dev c 246 0
#[root@BeagleBone ~]# 

#	Now our device file got created and registered with a major number and minor number.
#	Unload the driver using sudo rmmod



#4.2 Automatically Creating Device File
#################################################

		The automatic creation of device files can be handled with udev. Udev is the device manager for the Linux kernel that creates/removes device nodes in the /dev directory dynamically. Just follow the below steps.
		
		1. Include the header file linux/device.h and linux/kdev_t.h
		2. Create the struct Class
		3. Create Device with the class which is created by the above step

#	4.2.1 Create the class
--------------------------------------------
		This will create the struct class for our device driver. It will create a structure under/sys/class/.
		
		struct class * class_create (struct module *owner, const char *name);

		owner – pointer to the module that is to “own” this struct class

		name – pointer to a string for the name of this class	


This is used to create a struct class pointer that can then be used in calls to class_device_create.

Note, the pointer created here is to be destroyed when finished by making a call to class_destroy.


#	void class_destroy (struct class * cls);



#		4.2.2 Create Device
##############################################
	This function can be used by char device classes. A struct device will be created in sysfs, registered to the specified class.
	
	
		struct device *device_create (struct *class, struct device *parent, dev_t dev, void * drvdata, const char *fmt, ...);
		 

		class – pointer to the struct class that this device should be registered to

		parent – pointer to the parent struct device of this new device, if any

		devt – the dev_t for the char device to be added

		drvdata – the data to be added to the device for callbacks

		fmt – string for the device’s name

		... – variable arguments

		
		A “dev” file will be created, showing the dev_t for the device, if the dev_t is not 0,0. If a pointer to a parent struct device is passed in, the newly created struct device will be a child of that device in sysfs. The pointer to the struct device will be returned from the call. Any further sysfs files that might be required can be created using this pointer.

Note, you can destroy the device using device_destroy().

#		void device_destroy (struct class * class, dev_t devt);
If you don’t understand please refer to the below program, Then you will get some idea.

# Path -: LKI_BBB/0004_Creating_Device_File/Automatic_Method$

#*************************************************************************************************************************************
#				How To Test Driver -:
*************************************************************************************************************************************



#	Build the driver by using Makefile (sudo make) or if you are using the Beaglebone board, then you can use sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-.
#	Load the driver using sudo insmod
#	Check the device file using ls -l /dev/ | grep "device"



			[root@BeagleBone ~]# ls
			bin        driver.ko
			[root@BeagleBone ~]# insmod driver.ko 
			[ 6617.743703] driver: loading out-of-tree module taints kernel.
			[ 6617.750767] Major = 240 Minor = 0 
			[ 6617.754507] Kernel Module Inserted Successfully...
			[root@BeagleBone ~]# ls -l /dev/ | grep "device"
			crw-------    1 root     root      240,   0 Jan  1 01:50 Infinite_device
			[root@BeagleBone ~]# ls -l /dev/ | grep "device"
			crw-------    1 root     root      240,   0 Jan  1 01:50 Infinite_device
			[root@BeagleBone ~]# insmod driver.ko 
			insmod: can't insert 'driver.ko': File exists
			[root@BeagleBone ~]# rmmod driver.ko 
			[ 6639.157115] Kernel Module Removed Successfully...
			[root@BeagleBone ~]# insmod driver.ko 
			[ 6642.549268] Major = 240 Minor = 0 
			[ 6642.553100] Kernel Module Inserted Successfully...
			[root@BeagleBone ~]# ls -l /dev/ | grep "device"
			crw-------    1 root     root      240,   0 Jan  1 01:50 Infinite_device
			[root@BeagleBone ~]# 
			
			

