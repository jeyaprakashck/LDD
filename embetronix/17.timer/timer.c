
/************************************************************
 	CODE    : timer.c
	Author  : JP
	Details : Real device driver with timer functions
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
#include <linux/jiffies.h>
#include <linux/timer.h>


//creating the dev with custom major and minor number with dynamic method
dev_t dev = 0;
static struct class *devClass;
static struct cdev charDev;
char *kernelBuf;

//Timer variables
int count = 1;
static struct timer_list timerstruct;
static void timerFunc(struct timer_list *data){
	printk("Inside the timer function, count = %d \n", count++);
	mod_timer(&timerstruct, jiffies+msecs_to_jiffies(1000));
}
/*
 ** Function Prototype
 */
static int 	__init helloWorldInit(void);
static void	__exit helloWorldExit(void);
static int	helloDevOpen(struct inode *inode, struct file *file);
static int 	helloDevRelease(struct inode *inode, struct file *file);
static ssize_t 	helloDevRead(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t	helloDevWrite(struct file *file, const char __user *buf, size_t len, loff_t *offset);

static struct file_operations fops = {
	.owner 	= THIS_MODULE,
	.read 	= helloDevRead,
	.write	= helloDevWrite,
	.open	= helloDevOpen,
	.release= helloDevRelease,
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
 * init function
 */

static int __init helloWorldInit(void){
	/* allocating major number*/
	alloc_chrdev_region(&dev, 0, 1, "helloDev");
	printk(KERN_INFO "HelloWorld!!\n");
	printk(KERN_INFO "Major no = %d  Minor No = %d \n", MAJOR(dev), MINOR(dev));
	
	/*Creating struct class*/
	devClass = class_create(THIS_MODULE, "helloDevClass");
	if(IS_ERR(devClass)){
			pr_err("cannot create struct class for helloDevClass\n");
			goto r_class;
	}

	/*creating devices*/
	if(IS_ERR(device_create(devClass, NULL, dev, NULL, "hellodev"))){
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
	//timer setup
	timer_setup(&timerstruct, timerFunc, 0);
	mod_timer(&timerstruct, jiffies+msecs_to_jiffies(1000));
	
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
	del_timer(&timerstruct);
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
