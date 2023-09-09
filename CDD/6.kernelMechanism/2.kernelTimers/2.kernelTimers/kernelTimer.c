#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/module.h>
#define TIMEOUT 5000


MODULE_LICENSE("GPL");

dev_t dev;
static struct class *kernelTimerClass;
static struct cdev kernelTimerCdev;
static struct file_operations fops = {
	.owner = THIS_MODULE,
};

static struct timer_list __timer;
void timerCallback(struct timer_list *timer){
	printk("Timer callback function called\n");
	mod_timer(&__timer, (jiffies + msecs_to_jiffies(TIMEOUT)));
}

int init_module(void){
	printk("hello\n");
	alloc_chrdev_region(&dev, 0, 1, "kernelTimerDev");
	printk("Major : %d, Minor : %d\n", MAJOR(dev), MINOR(dev));
	cdev_init(&kernelTimerCdev, &fops);
	cdev_add(&kernelTimerCdev, dev, 1);
	kernelTimerClass = class_create(THIS_MODULE, "kernelTimerClass");
	device_create(kernelTimerClass, NULL, dev, NULL, "kernelTimerDevice");
	
	timer_setup(&__timer, timerCallback, 0);
	mod_timer(&__timer, jiffies + msecs_to_jiffies(TIMEOUT));
	
	printk("module loaded \n");
	return 0;
} 
