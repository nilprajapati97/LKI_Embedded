#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/uaccess.h>
#include<linux/kernel.h>
#include<linux/kdev_t.h>
#include<linux/slab.h>

/* Declarations */

static int skull_open(struct inode *inode, struct file *file);
static int skull_release(struct inode *inode, struct file *file);
ssize_t skull_read(struct file *flip, char __user *buf, size_t len, loff_t *off);
ssize_t skull_write(struct file *flip, const char __user *buf, size_t len, loff_t *off);


/* device number */
dev_t device_number;

/* class var */
static struct class *dev_class;

/* File Operation Structure */
static struct file_operations skull_fops = {
	.owner = THIS_MODULE,
	.read = skull_read,
	.write = skull_write,
	.open = skull_open,
	.release =  skull_release
};

/* cdev structure */
struct cdev skull_cdev;

/* kernel memory act as device */
#define MEM_SIZE 1024
uint8_t *kernel_buffer;


static int skull_open(struct inode *inode, struct file *file)
{
	if( (kernel_buffer = kmalloc(MEM_SIZE,GFP_KERNEL)) == 0){
		pr_info("kmalloc() failed can not allocate memory\n");
		return -1;
	}	
	pr_info("kmalloc() allocate memory device open\n");
	return 0;
}

static int skull_release(struct inode *inode, struct file *file)
{
	kfree(kernel_buffer);
	pr_info("kmalloc() dellocate memory device close\n");
	return 0;
}

ssize_t skull_read(struct file *flip, char __user *buf, size_t len, loff_t *off)
{
	copy_to_user(buf, kernel_buffer, MEM_SIZE);
	pr_info("copy_to_user() device read()\n");
	return MEM_SIZE;
}

ssize_t skull_write(struct file *flip,const char __user *buf, size_t len, loff_t *off)
{
	copy_from_user(kernel_buffer, buf, len);
	pr_info("copy_from_user() device write()\n");
	return len;
}

/* Driver init function */
static int __init char_dd_init(void){
	
	/* Get the dynamic Device number*/
	if(alloc_chrdev_region(&device_number,0,1,"skull")<0){
		pr_info("alloc_chrdev_region() failed\n");
		return -1;
	}

	/* Initialization of cdev */
	cdev_init(&skull_cdev,&skull_fops);

	/* add owner macro to owner field of cdev */
	skull_cdev.owner = THIS_MODULE;
	skull_cdev.ops = &skull_fops;

	/* Register the charecter device driver */
	if( cdev_add(&skull_cdev, device_number, 1) < 0)
	{
		pr_info("cdev_add() failed\n");
		goto r_class;
	}
	
	/* Register the class */
	if ( (dev_class = class_create(THIS_MODULE,"skull_class")) == NULL)
	{
		pr_info("class class_create() failed\n");
		goto r_class;
	}
	
	/* creating device */
	if ( (device_create(dev_class, NULL, device_number, NULL, "skull_device")) == NULL){
		pr_info("class class_create() failed\n");
		goto r_device;

	}
	
		

return 0;	// successfully initialized Device Driver

r_device:
	class_destroy(dev_class);

r_class:
	unregister_chrdev_region(device_number, 1);	
	return -1;
}


static void __exit char_dd_cleanup(void)
{
	/* 1  Device distory */
	device_destroy(dev_class, device_number);

	/* 2 class Distroy */
	class_destroy(dev_class);

	/* unregister cdev */
	cdev_del(&skull_cdev);
	/* Deallocation of chardriver region for free memory */
	unregister_chrdev_region(device_number, 1);

}

module_init(char_dd_init);
module_exit(char_dd_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(" Nil Prajapati <nil.prajapati97@yahoo.com > ");
MODULE_DESCRIPTION("Charecter Device Driver dummy");
MODULE_INFO(board,"Beaglebone Black");

