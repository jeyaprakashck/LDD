
/************************************************************
 	CODE    : workQueue.c
	Author  : JP
	Details : Real device driver with work delayed queue(static method) operations
************************************************************/



#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>		//kmalloc();
#include <linux/uaccess.h>	//copy to user, copy from user
#include <linux/ioctl.h>
#include <linux/workqueue.h>	//for workqueue functions
#include <linux/jiffies.h>	//for jiffies functions
#include <linux/kthread.h>
#include <linux/sched.h>

#define WRITE_CMD 	_IOW('A', 1, int*)
#define READ_CMD	_IOR('A', 2, int*)


//creating the dev with custom major and minor number with dynamic method
dev_t dev = 0;
static struct class *devClass;
static struct cdev charDev;
char *kernelBuf;
int value = 0;

//work queue variables
void workDelayQueueFunc(struct work_struct *work){
	printk("Inside the workQueue function after delay \n");
}

/* work delayed queue initialisation static method*/
static DECLARE_DELAYED_WORK(workDelayQueue, workDelayQueueFunc);

/*
 ** Function Prototype
 */
static int 	__init helloWorldInit(void);
static void	__exit helloWorldExit(void);
static int	helloDevOpen(struct inode *inode, struct file *file);
static int 	helloDevRelease(struct inode *inode, struct file *file);
static ssize_t 	helloDevRead(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t	helloDevWrite(struct file *file, const char __user *buf, size_t len, loff_t *offset);
static long 	helloDevIOCTL(struct file *file, unsigned int cmd, unsigned long arg);


static struct file_operations fops = {
	.owner 		= THIS_MODULE,
	.read 		= helloDevRead,
	.write		= helloDevWrite,
	.open		= helloDevOpen,
	.release	= helloDevRelease,
	.unlocked_ioctl = helloDevIOCTL,
};

/*
 * This function will be called whenever we open the device file
 */
static int helloDevOpen(struct inode *inode, struct file *file){
	pr_info("Driver open function is called\n");
	return 0;
}

/*
 * This function will be called whenever we close the device file
 */
static int  helloDevRelease(struct inode *inode, struct file *file){
	pr_info("Drvier close function is called\n");
	return 0;
}

/*
 * This function will be called whenever we write on device file
 */
static ssize_t helloDevWrite(struct file *file, const char __user *buf, size_t len, loff_t *offset){
	pr_info("Driver write function is called\n");
	if(copy_from_user(kernelBuf, buf, len))
		pr_err("Error while writing..\n");
	pr_info("Data written successfully\n");
	return len;
}

/*
 * This function will be called whenever we read from the device file
 */
static ssize_t helloDevRead(struct file *file, char __user *buf, size_t len, loff_t *offset){
       pr_info("Driver read function is called \n");
       if(copy_to_user(buf, kernelBuf, len))
	       pr_err("Error while reading\n");
       pr_info("Data read successfully\n");
       return len;
}


/*
 * This function will be called whenever user call IOCTL function 
 */
static long helloDevIOCTL(struct file *file, unsigned int cmd, unsigned long arg){
	switch(cmd){
		case WRITE_CMD:
			if(copy_from_user(kernelBuf, (int*)arg, 1024))
				pr_err("Data writtern Error\n");
			pr_info("Value is %s\n", kernelBuf);
			break;
		
		case READ_CMD:
			if(copy_to_user((int*)arg, kernelBuf, 1024))
				pr_err("Data read error\n");
			pr_info("Value read successfully\n");
			break;
		default:
			pr_info("default cmd\n");
			break;

	}
	return 0;
}




/*
 * init function
 */

static int __init helloWorldInit(void){
	/* allocating major number*/
	alloc_chrdev_region(&dev, 0, 1, "waitQueue");
	printk(KERN_INFO "HelloWorld!!\n");
	printk(KERN_INFO "Major no = %d  Minor No = %d \n", MAJOR(dev), MINOR(dev));
	
	/*Creating struct class*/
	devClass = class_create(THIS_MODULE, "waitQueueDevClass");
	if(IS_ERR(devClass)){
			pr_err("cannot create struct class for helloDevClass\n");
			goto r_class;
	}

	/*creating devices*/
	if(IS_ERR(device_create(devClass, NULL, dev, NULL, "waitQueuedev"))){
		pr_err("Cannot create the device file\n");
		goto r_devices;
	}
	
	/* Creating cdev structure */
	cdev_init(&charDev, &fops);

	/* Adding character device to the system */
	if((cdev_add(&charDev, dev, 1)) < 0){
		pr_err("Cannot add the device to the system \n");
		goto r_class;
	}
	
	/* Creating physical memory */
	if((kernelBuf = kmalloc(1024, GFP_KERNEL)) == 0){
		pr_info("cannot allocate memory\n");
		goto r_devices;
	}

	strcpy(kernelBuf, "Hi there");

	//schedule_work(&workQueue);
	schedule_delayed_work(&workDelayQueue, msecs_to_jiffies(2000));
	pr_info("Kernel Module inserted successfully\n");

	return 0;


r_class:
	unregister_chrdev_region(dev, 1);
	return -1;

r_devices:
	class_destroy(devClass);

}

/*
 * exit function
 */

static void __exit helloWorldExit(void){
	kfree(kernelBuf);
	device_destroy(devClass, dev);
	class_destroy(devClass);
	cdev_del(&charDev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Exiting \n");

}

module_init(helloWorldInit);
module_exit(helloWorldExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JP");
MODULE_VERSION("1:1.0");
MODULE_DESCRIPTION("Simple Hello World Kernel module");
