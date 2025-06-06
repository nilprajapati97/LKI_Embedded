0005_File_Operations
==================================================



#1 Prerequisites
#2 Cdev structure and File Operations of Character drivers
#	2.1 cdev structure
#	2.2 File_Operations
		#2.2.1 read
		#2.2.2 write
		#2.2.3 ioctl
		#2.2.4 Open
		#2.2.5 release (close)
		#2.2.6 Example
#3 Cdev structure and File Operations Example
#4 Dummy Driver
#5 Makefile
#6 Testing the Device Driver
======================================================================

# 2 Cdev structure and File Operations of Character drivers
	If we want to open, read, write, and close we need to register some structures to the driver.
#  cdev structure
--------------------
#	In Linux kernel "struct inode" structure is used to represent files. Therefore, it is different from the file structure that represents an open file descriptor. There can be numerous file structures representing multiple open descriptors on a single file, but they all point to a single inode structure.

#	The "inode" structure contains a great deal of information about the file. As a general rule, "cdev" structure is useful for writing driver code:

#	struct cdev is one of the elements of the inode structure. As you probably may know already, an inode structure is used by the kernel internally to represent files. The struct cdev is the kernel’s internal structure that represents char devices.  This field contains a pointer to that structure when the inode refers to a char device file.


				struct cdev { 
				    struct kobject kobj; 
				    struct module *owner; 
				    const struct file_operations *ops; 
				    struct list_head list; 
				    dev_t dev; 
				    unsigned int count; 
				};
				
				
This is cdev structure. Here we need to fill the two fields,

#1.	file_operation (This we will see after this cdev structure)
#2.	owner (This should be THIS_MODULE)

There are two ways of allocating and initializing one of these structures.

#1. Runtime Allocation
#2. Own allocation

If you wish to obtain a standalone cdev structure at runtime, you may do so with code such as:

				#struct cdev *my_cdev = cdev_alloc( );
				#my_cdev->ops = &my_fops;
				
				
Or else you can embed the cdev structure within a device-specific structure of your own by using the below function.


				void cdev_init(struct cdev *cdev, struct file_operations *fops);
				
Once the cdev structure is set up with file_operations and owner, the final step is to tell the kernel about it with a call to:
		
				int cdev_add(struct cdev *dev, dev_t num, unsigned int count);
		
Where,

dev is the cdev structure,

num is the first device number to which this device responds, and

count is the number of device numbers that should be associated with the device. Often count is one, but there are situations where it makes sense to have more than one device number correspond to a specific device.

If this function returns a negative error code, your device has not been added to the system. So check the return value of this function.



After a call to cdev_add(), your device is immediately alive. All functions you defined (through the file_operations structure) can be called.

To remove a char device from the system, call:

				void cdev_del(struct cdev *dev);
Clearly, you should not access the cdev structure after passing it to cdev_del.


#						2.2 File_Operations
------------------------------------------------------------------------------------------------------
	
	The file_operations structure is how a char driver sets up this connection. The structure, (defined in <linux/fs.h>), is a collection of function pointers. Each open file is associated with its own set of functions. The operations are mostly in charge of implementing the system calls and are, therefore, named open, read, and so on.

A file_operations structure is called fops. Each field in the structure must point to the function in the driver that implements a specific operation or have to left NULL for unsupported operations. The whole structure is mentioned below snippet.


						struct file_operations {
						    struct module *owner;
						    loff_t (*llseek) (struct file *, loff_t, int);
						    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
						    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
						    ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
						    ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
						    int (*iterate) (struct file *, struct dir_context *);
						    int (*iterate_shared) (struct file *, struct dir_context *);
						    unsigned int (*poll) (struct file *, struct poll_table_struct *);
						    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
						    long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
						    int (*mmap) (struct file *, struct vm_area_struct *);
						    int (*open) (struct inode *, struct file *);
						    int (*flush) (struct file *, fl_owner_t id);
						    int (*release) (struct inode *, struct file *);
						    int (*fsync) (struct file *, loff_t, loff_t, int datasync);
						    int (*fasync) (int, struct file *, int);
						    int (*lock) (struct file *, int, struct file_lock *);
						    ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
						    unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
						    int (*check_flags)(int);
						    int (*flock) (struct file *, int, struct file_lock *);
						    ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
						    ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
						    int (*setlease)(struct file *, long, struct file_lock **, void **);
						    long (*fallocate)(struct file *file, int mode, loff_t offset,
							      loff_t len);
						    void (*show_fdinfo)(struct seq_file *m, struct file *f);
						#ifndef CONFIG_MMU
						    unsigned (*mmap_capabilities)(struct file *);
						#endif
						    ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
							    loff_t, size_t, unsigned int);
						    int (*clone_file_range)(struct file *, loff_t, struct file *, loff_t,
							    u64);
						    ssize_t (*dedupe_file_range)(struct file *, u64, u64, struct file *,
							    u64);
						};


This file_operations structure contains many fields. But we will concentrate on very basic functions. Below we will see some fields explanation.


# struct module *owner:

The first file_operations field is not an operation at all; it is a pointer to the module that “owns” the structure. This field is used to prevent the module from being unloaded while its operations are in use. Almost all the time, it is simply initialized to THIS_MODULE, a macro defined in <linux/module.h>.

# read
					ssize_t (*read) (struct file *, char _ _user *, size_t, loff_t *);

This is used to retrieve data from the device. A null pointer in this position causes the read system call to fail with -EINVAL (“Invalid argument”). A non-negative return value represents the number of bytes successfully read (the return value is a “signed size” type, usually the native integer type for the target platform).


# write

					ssize_t (*write) (struct file *, const char _ _user *, size_t, loff_t *);
	
	It is used to sends the data to the device. If NULL -EINVAL is returned to the program calling the write system call. The return value, if non-negative, represents the number of bytes successfully written.	
	
	
#ioctl

					int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);

	The ioctl system call offers a way to issue device-specific commands (such as formatting a track of a floppy disk, which is neither reading nor writing). Additionally, a few ioctl commands are recognized by the kernel without referring to the fops table. If the device doesn’t provide an ioctl method, the system call returns an error for any request that isn’t predefined (-ENOTTY, “No such ioctl for device”). You can find the IOCTL tutorial here.

# Open
					int (*open) (struct inode *, struct file *);
	Though this is always the first operation performed on the device file, the driver is not required to declare a corresponding method. If this entry is NULL, opening the device always succeeds, but your driver isn’t notified.

#release (close)

					int (*release) (struct inode *, struct file *);

	This operation is invoked when the file structure is being released. Like open, release can be NULL.





Example
=========

					static struct file_operations fops =
					{
					.owner          = THIS_MODULE,
					.read           = etx_read,
					.write          = etx_write,
					.open           = etx_open,
					.release        = etx_release,
					};



If you want to understand the complete flow, just have a look at our dummy driver.

#						3.Cdev Operations Example
------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------



# 4. Dummy Driver
---------------

===================================================================================================
							/***************************************************************************/
							*  \file       driver.c
							*
							*  \details    Simple Linux device driver (File Operations)
							*
							*  \author     EmbeTronicX
							*
							*  \Tested with Linux linux-4.19.103/
							*
							*******************************************************************************/
							#include <linux/kernel.h>
							#include <linux/init.h>
							#include <linux/module.h>
							#include <linux/kdev_t.h>
							#include <linux/fs.h>
							#include <linux/cdev.h>
							#include <linux/device.h>

							dev_t dev = 0;
							static struct class *dev_class;
							static struct cdev etx_cdev;

							/*
							** Function Prototypes
							*/
							static int      __init etx_driver_init(void);
							static void     __exit etx_driver_exit(void);
							static int      etx_open(struct inode *inode, struct file *file);
							static int      etx_release(struct inode *inode, struct file *file);
							static ssize_t  etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
							static ssize_t  etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);

							static struct file_operations fops =
							{
							    .owner      = THIS_MODULE,
							    .read       = etx_read,
							    .write      = etx_write,
							    .open       = etx_open,
							    .release    = etx_release,
							};

							/*
							** This function will be called when we open the Device file
							*/
							static int etx_open(struct inode *inode, struct file *file)
							{
								pr_info("Driver Open Function Called...!!!\n");
								return 0;
							}

							/*
							** This function will be called when we close the Device file
							*/
							static int etx_release(struct inode *inode, struct file *file)
							{
								pr_info("Driver Release Function Called...!!!\n");
								return 0;
							}

							/*
							** This function will be called when we read the Device file
							*/
							static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
							{
								pr_info("Driver Read Function Called...!!!\n");
								return 0;
							}

							/*
							** This function will be called when we write the Device file
							*/
							static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
							{
								pr_info("Driver Write Function Called...!!!\n");
								return len;
							}

							/*
							** Module Init function
							*/
							static int __init etx_driver_init(void)
							{
								/*Allocating Major number*/
								if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
									pr_err("Cannot allocate major number\n");
									return -1;
								}
								pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

								/*Creating cdev structure*/
								cdev_init(&etx_cdev,&fops);

								/*Adding character device to the system*/
								if((cdev_add(&etx_cdev,dev,1)) < 0){
								    pr_err("Cannot add the device to the system\n");
								    goto r_class;
								}

								/*Creating struct class*/
								if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
								    pr_err("Cannot create the struct class\n");
								    goto r_class;
								}

								/*Creating device*/
								if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
								    pr_err("Cannot create the Device 1\n");
								    goto r_device;
								}
								pr_info("Device Driver Insert...Done!!!\n");
							      return 0;

							r_device:
								class_destroy(dev_class);
							r_class:
								unregister_chrdev_region(dev,1);
								return -1;
							}

							/*
							** Module exit function
							*/
							static void __exit etx_driver_exit(void)
							{
								device_destroy(dev_class,dev);
								class_destroy(dev_class);
								cdev_del(&etx_cdev);
								unregister_chrdev_region(dev, 1);
								pr_info("Device Driver Remove...Done!!!\n");
							}

							module_init(etx_driver_init);
							module_exit(etx_driver_exit);

							MODULE_LICENSE("GPL");
							MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
							MODULE_DESCRIPTION("Simple Linux device driver (File Operations)");
							MODULE_VERSION("1.3");
========================================================================================================


# 5 Makefile
==================

------------------------------------------------------------------------------------------------------------------------------------------------------
							obj-m += driver.o
							 
							ifdef ARCH
							  #You can update your Beaglebone path here.
							  KDIR = /home/embetronicx/BBG/tmp/lib/modules/5.10.65/build
							else
							  KDIR = /lib/modules/$(shell uname -r)/build
							endif
							 
							 
							all:
							  make -C $(KDIR)  M=$(shell pwd) modules
							 
							clean:
							  make -C $(KDIR)  M=$(shell pwd) clean

-----------------------------------------------------------------------------------------------------------------------------------------------------------



#Testing the Device Driver
--------------------------------------------------


#1. Build the driver by using Makefile (sudo make) or if you are using the Beaglebone board, then you can use sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-.

#2. Load the driver using sudo insmod

#3. Do echo 1 > /dev/etx_device

cho will open the driver and write 1 into the driver and finally close the driver. So if I do echo to our driver, it should call the open, write and release functions. Just check.

#4							linux@embetronicx-VirtualBox:/home/driver/driver#  echo 1 > /dev/etx_device
							
Now Check using dmesg
#5							linux@embetronicx-VirtualBox:/home/driver/driver$ dmesg
							[19721.611967] Major = 246 Minor = 0
							[19721.618716] Device Driver Insert...Done!!!
							[19763.176347] Driver Open Function Called...!!!
							[19763.176363] Driver Write Function Called...!!!
							[19763.176369] Driver Release Function Called...!!!
							

#6   Do cat > /dev/etx_device

cat command will open the driver, read the driver, and close the driver. So if I do cat to our driver, it should call the open, read, and release functions. Just check.


#7							linux@embetronicx-VirtualBox:/home/driver/driver#  cat /dev/etx_device
							
Now Check using dmesg

#8							linux@embetronicx-VirtualBox:/home/driver/driver$ dmesg
							[19763.176347] Driver Open Function Called...!!!
							[19763.176363] Driver Read Function Called...!!!
							[19763.176369] Driver Release Function Called...!!!
	

#9 Unload the driver using sudo rmmod

	Instead of doing echo and cat command in the terminal you can also use open(), read(), write(), close() system calls from user-space applications.	
	
*********************************************************************************************************************************************************************************************

	
	
	
							
							
							
							
							


							
							
							
							
							
							
							
							
							
							
							
							
							
							
							















	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
					
					
















	
	
















		













				
				
				
