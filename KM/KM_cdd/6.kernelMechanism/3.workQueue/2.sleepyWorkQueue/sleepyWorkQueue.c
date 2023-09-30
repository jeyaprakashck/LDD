#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/workqueue.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");

dev_t dev;
static struct class *sleepyWorkQueueClass;
static struct cdev sleepyWorkQueueCdev;

static struct work_struct ws;
static struct timer_list __timer;
DECLARE_WAIT_QUEUE_HEAD(wq);
static int count;
static int dataPresent = 0;

static int openFunc(struct inode *inode, struct file *file);
static int closeFunc(struct inode *inode, struct file *file);
static ssize_t readFunc(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t writeFunc(struct file *file, const char __user *buf, size_t len, loff_t *offset);
static void workQueueFunc(struct work_struct *work);
static void delayFunc(struct timer_list *timer);

static struct file_operations fops = {
	.owner	= THIS_MODULE,
	.open	= openFunc,
	.release= closeFunc,
	.read	= readFunc,
	.write	= writeFunc,
};


static int openFunc(struct inode *inode, struct file *file){
	printk("driver opened\n");
	return 0;
}

static int closeFunc(struct inode *inode, struct file *file){
	printk("driver closed\n");
	return 0;
}

ssize_t readFunc(struct file *file, char __user *buf, size_t len, loff_t *offset){
	printk("read function is called\n");

	printk("process : %d (%s) going to sleep\n", current->pid, current->comm);
	if(wait_event_interruptible(wq, dataPresent == 1)){
		printk("wait interrupted by signal\n");
		return -1;
	}
	printk("awoke %d (%s), dataPresent=%d \n", current->pid, current->comm, dataPresent);
	return 0;
} 

ssize_t writeFunc(struct file *file, const char __user *buf, size_t len, loff_t *offset){
	printk("write function is called \n");
	schedule_work(&ws);
	printk("awakening the readers\n");
	dataPresent = 1;
	wake_up_interruptible(&wq);
	return len;
}

static void workQueueFunc(struct work_struct *work){
	printk("Now setting up a timer ..\n");
	timer_setup(&__timer, delayFunc, 0);
	mod_timer(&__timer, jiffies + msecs_to_jiffies(2000));
}

static void delayFunc(struct timer_list *timer){
	printk("inside the delay function \n");
	timer_setup(&__timer, delayFunc, 0);
	mod_timer(&__timer, jiffies + msecs_to_jiffies(2000));
}
	

int init_module(void){
	alloc_chrdev_region(&dev, 0, 1, "sleepyWorkQueueMajor");
	cdev_init(&sleepyWorkQueueCdev, &fops);
	cdev_add(&sleepyWorkQueueCdev, dev, 1);
	sleepyWorkQueueClass = class_create(THIS_MODULE, "SleepyWorkQueueClass");
	device_create(sleepyWorkQueueClass, NULL, dev, NULL, "sleepyWorkQueueDevice");
	printk("Module loaded\n");
	INIT_WORK(&ws, workQueueFunc);
	return 0;
}

void cleanup_module(void){
	del_timer(&__timer);
	flush_scheduled_work();
	device_destroy(sleepyWorkQueueClass, dev);
	class_destroy(sleepyWorkQueueClass);
	cdev_del(&sleepyWorkQueueCdev);
	unregister_chrdev_region(dev, 1);
	printk("unloaded\n");
}
	
