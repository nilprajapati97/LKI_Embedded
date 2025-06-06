/* Disk on RAM Driver */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/genhd.h> // For basic block driver framework
#include <linux/blkdev.h> // For at least, struct block_device_operations
#include <linux/hdreg.h> // For struct hd_geometry
#include <linux/errno.h>

#include "ram_device.h"

#define RB_FIRST_MINOR 0
#define RB_MINOR_CNT 16

static u_int rb_major = 0;

/*
 * The internal structure representation of our Device
 */
static struct rb_device
{
	/* Size is the size of the device (in sectors) */
	sector_t size;
	/* For exclusive access to our request queue */
	spinlock_t lock;
	/* Our request queue */
	struct request_queue *queue;
	/* This is kernel's representation of an individual disk device */
	struct gendisk *disk;
} rb_dev;

static int rb_open(struct block_device *bdev, fmode_t mode)
{
	unsigned unit = iminor(bdev->bd_inode);

	printk(KERN_INFO "rb: Device is opened\n");
	printk(KERN_INFO "rb: Inode number is %d\n", unit);

	if (unit > RB_MINOR_CNT)
		return -ENODEV;
	return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0))
static int rb_close(struct gendisk *disk, fmode_t mode)
{
	printk(KERN_INFO "rb: Device is closed\n");
	return 0;
}
#else
static void rb_close(struct gendisk *disk, fmode_t mode)
{
	printk(KERN_INFO "rb: Device is closed\n");
}
#endif

//TODO 17: Populate the disk geometry
// Populate the fields heads, cylinders, sectors and start
static int rb_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	geo->heads = 1;
	geo->cylinders = 32;
	geo->sectors = 32;
	geo->start = 0;
	return 0;
}

/*
 * Actual Data transfer
 */
static int rb_transfer(struct request *req)
{
	//struct rb_device *dev = (struct rb_device *)(req->rq_disk->private_data);

	int dir = rq_data_dir(req);
	sector_t start_sector = blk_rq_pos(req);
	unsigned int sector_cnt = blk_rq_sectors(req);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0))
#define BV_PAGE(bv) ((bv)->bv_page)
#define BV_OFFSET(bv) ((bv)->bv_offset)
#define BV_LEN(bv) ((bv)->bv_len)
	struct bio_vec *bv;
#else
#define BV_PAGE(bv) ((bv).bv_page)
#define BV_OFFSET(bv) ((bv).bv_offset)
#define BV_LEN(bv) ((bv).bv_len)
	struct bio_vec bv;
#endif
	struct req_iterator iter;

	sector_t sector_offset;
	unsigned int sectors;
	u8 *buffer;

	int ret = 0;

	//printk(KERN_DEBUG "rb: Dir:%d; Sec:%lld; Cnt:%d\n", dir, (unsigned long long)(start_sector), sector_cnt);

	sector_offset = 0;
	rq_for_each_segment(bv, req, iter)
	{
		//TODO 13: Initialize the buffer to point to data in bio_vec
		buffer = page_address(BV_PAGE(bv)) + BV_OFFSET(bv);
		if (BV_LEN(bv) % RB_SECTOR_SIZE != 0)
		{
			printk(KERN_ERR "rb: Should never happen: "
				"bio size (%d) is not a multiple of RB_SECTOR_SIZE (%d).\n"
				"This may lead to data truncation.\n",
				BV_LEN(bv), RB_SECTOR_SIZE);
			ret = -EIO;
		}
		// TODO 14: Get the total number of sectors
		sectors = BV_LEN(bv) / RB_SECTOR_SIZE;
		printk(KERN_DEBUG "rb: Start Sector: %llu, Sector Offset: %llu; Buffer: %p; Length: %u sectors\n",
			(unsigned long long)(start_sector), (unsigned long long)(sector_offset), buffer, sectors);
		if (dir == WRITE) /* TODO 15: Write to the device */
		{
			ramdevice_write(start_sector + sector_offset, buffer, sectors);
		}
		else /* TODO 16: Read from the device */
		{
			ramdevice_read(start_sector + sector_offset, buffer, sectors);
		}
		sector_offset += sectors;
	}
	if (sector_offset != sector_cnt)
	{
		printk(KERN_ERR "rb: bio info doesn't match with the request info");
		ret = -EIO;
	}

	return ret;
}

/*
 * Represents a block I/O request for us to execute
 */
static void rb_request(struct request_queue *q)
{
	struct request *req;
	int ret;

	/* Gets the current request from the dispatch queue */
	while ((req = blk_fetch_request(q)) != NULL)
	{
		ret = rb_transfer(req);
		/* End the request */
		__blk_end_request_all(req, ret); // Equivalent to the following:
		//__blk_end_request(req, ret, blk_rq_bytes(req));
	}
}

/*
 * These are the file operations that performed on the ram block device
 */
// TODO 8: Populate the file_operations
static struct block_device_operations rb_fops =
{
	.owner = THIS_MODULE,
	.open = rb_open,
	.release = rb_close,
	.getgeo = rb_getgeo,
};

/*
 * This is the registration and initialization section of the ram block device
 * driver
 */
static int __init rb_init(void)
{
	sector_t size;

	//TODO 2: Initialize with DOR memory size
	size = ramdevice_init();
	if (size < 0)
	{
		return (int)(size);
	}
	rb_dev.size = size;

	/* Get Registered */
	//TODO 1: Register the block device with register_blkdev(major, name)
	rb_major = register_blkdev(0, "rb");
	if (rb_major <= 0)
	{
		printk(KERN_ERR "rb: Unable to get Major Number\n");
		ramdevice_cleanup();
		return -EBUSY;
	}

	/* Get a request queue (here queue is created) */
	spin_lock_init(&rb_dev.lock);
	//TODO 3: Initialize the request queue with blk_init_queue(request_fn, lock)
	rb_dev.queue = blk_init_queue(rb_request, &rb_dev.lock);
	if (IS_ERR(rb_dev.queue))
	{
		printk(KERN_ERR "rb: request queue allocation & initialization failure\n");
		unregister_blkdev(rb_major, "rb");
		ramdevice_cleanup();
		return PTR_ERR(rb_dev.queue);
	}

	/*
	 * TODO 4: Allocate the gendisk structure
	 * By using this memory allocation is involved,
	 * the minor number we need to pass bcz the device
	 * will support this much partitions
	 */
	rb_dev.disk = alloc_disk(RB_MINOR_CNT);
	if (!rb_dev.disk)
	{
		printk(KERN_ERR "rb: alloc_disk failure\n");
		blk_cleanup_queue(rb_dev.queue);
		unregister_blkdev(rb_major, "rb");
		ramdevice_cleanup();
		return -ENOMEM;
	}

	/* TODO 5: Setting the major number */
	rb_dev.disk->major = rb_major;
	/* TODO 6: Setting the first minor number */
	rb_dev.disk->first_minor = RB_FIRST_MINOR;
	/* TODO 7: Initializing the device operations */
	rb_dev.disk->fops = &rb_fops;
	/* TODO 9: Driver-specific own internal data */
	rb_dev.disk->private_data = &rb_dev;
	/* TODO 10: Setting the queue*/
	rb_dev.disk->queue = rb_dev.queue;
	/*
	 * You do not want partition information to show up in
	 * cat /proc/partitions set this flags
	 */
	//rb_dev.disk->flags = GENHD_FL_SUPPRESS_PARTITION_INFO;
	sprintf(rb_dev.disk->disk_name, "rb");
	/* Setting the capacity of the device in its gendisk structure */
	set_capacity(rb_dev.disk, rb_dev.size);

	/* TODO 11: Adding the disk to the system with add_disk */
	add_disk(rb_dev.disk);
	/* Now the disk is "live" */
	printk(KERN_INFO "rb: Ram Block driver initialised (%llu sectors; %llu bytes)\n",
		(unsigned long long)(rb_dev.size), (unsigned long long)(rb_dev.size * RB_SECTOR_SIZE));

	return 0;
}
/*
 * This is the unregistration and uninitialization section of the ram block
 * device driver
 */
static void __exit rb_cleanup(void)
{
	// TODO 18: Delete the gendisk with del_gendisk
	del_gendisk(rb_dev.disk);
	
	// TODO 19: Deallocate the gendisk with put_disk
	put_disk(rb_dev.disk);
	// TODO 20: De-initialize the queu with blk_cleanup_queue
	blk_cleanup_queue(rb_dev.queue);
	// TODO 21: Finally unregister the block device with unregister_blkdev
	unregister_blkdev(rb_major, "rb");
	// TODO 22: Free up the ramdisk memory
	ramdevice_cleanup();	
}

module_init(rb_init);
module_exit(rb_cleanup);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Ram Block Driver");
MODULE_ALIAS_BLOCKDEV_MAJOR(rb_major);
