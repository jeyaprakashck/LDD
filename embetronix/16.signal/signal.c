
/************************************************************
 	CODE    : signal.c
	Author  : JP
	Details : Real device driver with signal operations. This
	module will sent signals to user program when we read the device
	file/

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
#include <linux/sched/signal.h>
#include <linux/signal.h>
#include <linux/signal_types.h>

#define REGISTER_CURRENT_TASK 	_IOW('A', 1, int*)

#define SIGNO 44
//creating the dev with custom major and minor number with dynamic method
dev_t dev = 0;
static struct class *devClass;
static struct cdev charDev;
char *kernelBuf;

//signalling varaiables
static struct task_struct *task = NULL;
int value = 0;

//work queue variables
static struct work_struct workQueue;
void workQueueFunc(struct work_struct *work){
	struct siginfo info;
	printk("Inside the workQueue function and sending signal to user application \n");

	/* sending signal to user program */
	memset(&info, 0, sizeof(struct siginfo));
	info.si_signo = SIGNO;
	info.si_code  = SI_QUEUE;
	info.si_int   = 1;  
	
	if(task!=NULL){
		printk("Sending signal to user application\n");
		send_sig_info(SIGNO, &info, task);
	}

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
	struct task_struct *temp = get_current();
	//delating task
	if(temp == task)
		task = NULL;
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
	schedule_work(&workQueue);
       return 0;
}


/*
 * This function will be called whenever user call IOCTL function 
 */
static long helloDevIOCTL(struct file *file, unsigned int cmd, unsigned long arg){
	printk("inside the IOCTL\n");
	if(cmd == REGISTER_CURRENT_TASK){
		printk("Registering current task\n");
		//registering current user task 
		task = get_current();
	}
	return 0;
}




/*
 * init function
 */

static int __init helloWorldInit(void){
	/* allocating major number*/
	alloc_chrdev_region(&dev, 0, 1, "signal");
	printk(KERN_INFO "HelloWorld!!\n");
	printk(KERN_INFO "Major no = %d  Minor No = %d \n", MAJOR(dev), MINOR(dev));
	
	/*Creating struct class*/
	devClass = class_create(THIS_MODULE, "signalDevClass");
	if(IS_ERR(devClass)){
			pr_err("cannot create struct class for helloDevClass\n");
			goto r_class;
	}

	/*creating devices*/
	if(IS_ERR(device_create(devClass, NULL, dev, NULL, "signaldev"))){
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

	/* work queue initialisation in dynamic method */
	INIT_WORK(&workQueue, workQueueFunc);
	//schedule_delayed_work(&workQueue, msecs_to_jiffies(2000));
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
