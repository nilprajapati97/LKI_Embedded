#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>

/* Add your variables here */

static dev_t first; // Global variable for the first device number

static struct cdev c_dev; 



static int my_open(struct inode *i, struct file *f)
{
	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	return 0;
}

static char c = 'A';

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	buf[0] = 'A' ; // giving crash 

	printk(KERN_INFO "Driver: In read - Buf: %p; Len: %zd; Off: %Ld\nData:\n", buf, len, *off);\

	if (*off == 0)
	{
		if (copy_to_user(buf, &c, 1))
		{
			return -EFAULT;
		}
		*off += 1;
		return 1;
	}
	else
		return 0;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Driver: In write - Buf: %p; Len: %zd; Off: %Ld\nData:\n", buf, len, *off);
	printk(KERN_INFO);

	if (copy_from_user(&c, buf + len - 1, 1))
	{
		return -EFAULT;
	}
	return len;
}


//TODO 3: Register the call backs from open, release, read, write
static struct file_operations driver_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write

};

static int __init ofcd_init(void) /* Constructor */
{
	int ret = 0;

	printk(KERN_INFO "Hello: ofcd registered");

	if(ret = alloc_chrdev_region(&first, 0, 3, "Infinite_first_driver") < 0)
	{
		return ret;
	}

	//TODO 1: Register the char driver with name ofcd with 1 minor
	printk(KERN_INFO "<Major, Minor>: <%d, %d>\n", MAJOR(first), MINOR(first));

	cdev_init(&c_dev, &driver_fops);
	cdev_add(&c_dev, first, 1);   // 

	return 0;
}



static void __exit ofcd_exit(void) /* Destructor */
{
	//TODO 2: //unregister the char driver
	
	unregister_chrdev_region(first, 3);
	
	printk(KERN_INFO "Bye: ofcd unregistered");
}

module_init(ofcd_init);
module_exit(ofcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(" NIL PRAJAPATI < nil.prajapati97@yahoo.com >");
MODULE_DESCRIPTION("Our First Character Driver");
