#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>		//kmalloc
#include <linux/uaccess.h>	//copy to user and copy from user

MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("Real device driver");
MODULE_LICENSE("GPL");

dev_t dev;
static struct cdev cdevStructure;	//This is used to create and add char dev to the system
static struct class *devClass;		//This is used to load device file in /dev (mknod)
char *kernelBuffer;

/*
**Function Prototype
*/
static ssize_t readFunc(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t writeFunc(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static int openFunc(struct inode *inode, struct file *file);
static int releaseFunc(struct inode *inode, struct file *file);


static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = readFunc,
	.write = writeFunc,
	.open = openFunc,
	.release = releaseFunc
};


static ssize_t readFunc(struct file *filp, char __user *buf, size_t len, loff_t *off){
	printk("Driver read function is called \n");
	copy_to_user(buf, kernelBuffer, 1024);
	return sizeof(kernelBuffer);
}

static ssize_t writeFunc(struct file *filp, const char __user *buf, size_t len, loff_t *off){
	printk("Driver write function is called \n");
	copy_from_user(kernelBuffer, buf, len);
	return len;
}

static int openFunc(struct inode *inode, struct file *file){
	printk("Driver open function is called \n");
	return 0;
}

static int releaseFunc(struct inode *inode, struct file *file){
	printk("Driver release function is called \n");
	return 0;
}

static int helloInit(void){
	/*allocating Major number dynamically */
	alloc_chrdev_region(&dev, 1, 1, "dynamicMajorNoExample");
	printk("Kernel module is loaded \n");
	printk("Major = %d Minor = %d\n, dev value = %d\n", MAJOR(dev), MINOR(dev), dev);
	printk(KERN_INFO "%s : %s : %d\n", __FILE__, __func__, __LINE__);
	
	/* Creating cdev structure */
	cdev_init(&cdevStructure, &fops);
	
	/* Adding character device to the system */
	cdev_add(&cdevStructure, dev, 1);

	/* device class creation to create device file inside /dev folder */
	devClass = class_create(THIS_MODULE, "charDeviceTestingClass");
	
	/* creating device file */
	device_create(devClass, NULL, dev, NULL, "charDeviceTesting");
	
	/* Creating memory in kernel */
	kernelBuffer = kmalloc(1024, GFP_KERNEL);
	strcpy(kernelBuffer, "HelloJP");
	
	return 0;

}

static void helloExit(void){
	device_destroy(devClass, dev);
	class_destroy(devClass);
	cdev_del(&cdevStructure);
	unregister_chrdev_region(dev, 1);
	printk("Kernel module is unloaded \n");
	printk(KERN_INFO "%s : %s : %d\n", __FILE__, __func__, __LINE__);
}

module_init(helloInit);
module_exit(helloExit);

