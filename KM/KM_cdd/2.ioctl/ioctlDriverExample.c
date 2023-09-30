#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>		//kmalloc
#include <linux/uaccess.h>	//copy to user and copy from user
#include <linux/kdev_t.h>

#define MAGIC_NUMBER		 'a'
#define SET_BAUD_RATE		_IOWR(MAGIC_NUMBER, 8, int)
#define	SET_NO_STOP_BITS	_IOWR(MAGIC_NUMBER, 9, int)
#define SET_DIRECTION_WRITE	_IOW(MAGIC_NUMBER, 10, int)

MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("ioctl device driver");
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
static long ioctlFunc(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = readFunc,
	.write = writeFunc,
	.open = openFunc,
	.release = releaseFunc,
	.unlocked_ioctl = ioctlFunc
};


static ssize_t readFunc(struct file *filp, char __user *buf, size_t len, loff_t *off){
	
	printk("Driver read function is called \n");
	copy_to_user(buf, kernelBuffer, 1024);
	return 1024;
}

static ssize_t writeFunc(struct file *filp, const char __user *buf, size_t len, loff_t *off){
	memset(kernelBuffer, 0, sizeof(kernelBuffer));
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

static long ioctlFunc(struct file *file, unsigned int cmd, unsigned long arg){
	printk("Inside the ioctl function\n");
	switch(cmd){
		case SET_BAUD_RATE:
			printk("Setting the baud rate to %ld\n", arg);
			printk("int value of SET_BAUD_RATE = %d\n", SET_BAUD_RATE);
			break;
		case SET_NO_STOP_BITS:
			printk("Setting the no of stop bits to %ld\n", arg);
			break;
		case SET_DIRECTION_WRITE:
			printk("The direction is set to write\n");
			break;
		default:
			printk("command not found \n");
			printk("int value of SET_BAUD_RATE = %d\n", SET_BAUD_RATE);
			return -1;
	}
	return 0;
}

static int helloInit(void){
	/*allocating Major number dynamically */
	alloc_chrdev_region(&dev, 1, 1, "ioctlExample");
	printk("Kernel module is loaded \n");
	printk("Major = %d Minor = %d\n, dev value = %d\n", MAJOR(dev), MINOR(dev), dev);
	printk(KERN_INFO "%s : %s : %d\n", __FILE__, __func__, __LINE__);
	
	/* Creating cdev structure */
	cdev_init(&cdevStructure, &fops);
	
	/* Adding character device to the system */
	cdev_add(&cdevStructure, dev, 1);

	/* device class creation to create device file inside /dev folder */
	devClass = class_create(THIS_MODULE, "ioctlExampleClass");
	
	/* creating device file */
	device_create(devClass, NULL, dev, NULL, "ioctlExampleDriver");
	
	/* Creating memory in kernel */
	kernelBuffer = kmalloc(1024, GFP_KERNEL);
	
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

