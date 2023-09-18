#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>

//Interrupt parameters
#define IRQ1	1
int interruptCount = 0;
irqreturn_t keyboardHandler(int irqno, void *data);
int id;

//tasklet parameters
struct tasklet_struct taskletStruct;
void taskletFunc(unsigned long data);	//bottom half function

//waitqueue parameters (dynamic method)
wait_queue_head_t waitQueue;
int waitQueueFlag = 0;

//device file parameters
dev_t dev;
static struct cdev interruptCdevStructure;
static struct class *interruptClass;
static int openFunc(struct inode *inode, struct file *file);
static int closeFunc(struct inode *inode, struct file *file);
static ssize_t readFunc(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t writeFunc(struct file *file, const char __user *buf, size_t count, loff_t *offset);
static struct file_operations fops = {
	.open 	 = openFunc,
	.release = closeFunc,
	.read	 = readFunc,
	.write	 = writeFunc,
};

//Function definitions

void taskletFunc(unsigned long data){
	printk("Inside the tasklet function(BottomHalf)\n");
	printk("Interrupt count = %d\n", interruptCount++);
	waitQueueFlag = 1;
	wake_up(&waitQueue);				//this function will unblock read operation
}

irqreturn_t keyboardHandler(int irqno, void *data){
	printk("Inside the keyboard handler\n");
	printk("Calling bottom half ...\n");
	tasklet_schedule(&taskletStruct);
	return IRQ_HANDLED;
}

static int openFunc(struct inode *inode, struct file *file){
	printk("file open func called\n");
	return 0;
}

static int closeFunc(struct inode *inode, struct file *file){
	printk("file close func called\n");
	return 0;
}

static ssize_t readFunc(struct file *file, char __user *buf, size_t count,loff_t *offset){
	printk("read function is called\n");
	printk("Blocking .... Waiting for keyboard event\n");
	waitQueueFlag = 0;
	wait_event(waitQueue, waitQueueFlag == 1);	//This function will wait unitll interrupt occurs
	waitQueueFlag = 0;
	return 0;
}

static ssize_t writeFunc(struct file *file, const char __user *buf, size_t count, loff_t *offset){
	printk("write func is called \n");
	return 0;
}
	
int __init init_module(void){
	printk("Loading the module\n");
	//interrupt initialization
	if(request_irq(IRQ1, keyboardHandler, IRQF_SHARED, "keyboardInterrupt", &id)){
		printk("Error during interrupt request\n");
		return -1;
	}
	
	taskletStruct.func = &taskletFunc;

	//creating device files
	alloc_chrdev_region(&dev, 0, 1, "interruptDevice");
	cdev_init(&interruptCdevStructure, &fops);
	cdev_add(&interruptCdevStructure, dev, 1);
	interruptClass = class_create(THIS_MODULE, "interruptClass");
	device_create(interruptClass, NULL, dev, NULL, "interruptDeviceExample");
	
	printk("Module loaded and interrupt requested\n");
	return 0;	
}

void __exit cleanup_module(void){
	free_irq(IRQ1, &id);
	wake_up(&waitQueue);
	device_destroy(interruptClass, dev);
	class_destroy(interruptClass);
	cdev_del(&interruptCdevStructure);
	unregister_chrdev_region(dev, 1);
	printk("interrupt module is unloaded\n");
}
		
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("Keyboard handler example with tasklet");


