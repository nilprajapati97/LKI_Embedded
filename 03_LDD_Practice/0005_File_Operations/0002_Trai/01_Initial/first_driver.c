#include <linux/module.h>
#include <linux/kernel.h>

static int __init driver_init(void) /* Constructor */
{
	printk(KERN_INFO "driver registered\n");
	return 0;
}

static void __exit driver_exit(void) /* Destructor */
{
	printk(KERN_INFO "driver degistered\n");
}

module_init(driver_init);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(" Nil Prajapati  < nil.prajapati97@yahoo.com >");
MODULE_DESCRIPTION("My First Driver");
