/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple linux driver (Manually Creating a Device file)
*
*  \author     NIL PRAJAPATI
*
*  \Tested with Linux linux-4.19.103 BBB
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
MODULE_AUTHOR("EmbeTronicX < nil.prajapati97@yahoo.com >");
MODULE_DESCRIPTION("Simple linux driver (Manually Creating a Device file)");
MODULE_VERSION("1.1");
