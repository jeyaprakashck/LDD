#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>		//kmalloc
#include <linux/uaccess.h>	//copy to user and copy from user
#include <linux/sched.h>
#include <linux/kdev_t.h>


MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("calculator(multiple) device driver");
MODULE_LICENSE("GPL");

dev_t dev;
dev_t devTemp;
static struct cdev cdevStructure[4];	//This is used to create and add char dev to the system
static struct class *devClass;		//This is used to load device file in /dev (mknod)

int addResult;
int subResult;
int mulResult;
int divResult;

/*
**Function Prototype
*/
static ssize_t readFuncADD(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t writeFuncADD(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static ssize_t readFuncSUB(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t writeFuncSUB(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static ssize_t readFuncMUL(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t writeFuncMUL(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static ssize_t readFuncDIV(struct file *filp,  char __user *buf, size_t len, loff_t *off);
static ssize_t writeFuncDIV(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static int openFunc(struct inode *inode, struct file *file);
static int releaseFunc(struct inode *inode, struct file *file);


static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = openFunc,
	.release = releaseFunc
};


static ssize_t readFuncADD(struct file *filp, char __user *buf, size_t len, loff_t *off){
	printk("in ADD Read function. The sum is %d\n", addResult);
	copy_to_user(buf, (const char*)&addResult, 4);
	return 0;
}

static ssize_t writeFuncADD(struct file *filp, const char __user *buf, size_t len, loff_t *off){
	printk("ADD : Driver write function is called \n");
	char kernelBuffer[20];
	int *a = NULL;
	int *b = NULL;
	printk("ADD Driver read function is called \n");
	copy_from_user(kernelBuffer, buf, len);
	a = (int*)kernelBuffer;
	b = (int*)&kernelBuffer[4];
	addResult = *a + *b;
	printk("a = %d, b = %d,  a+b = %d\n", *a, *b, addResult);
	return 0;
}

static ssize_t readFuncSUB(struct file *filp, char __user *buf, size_t len, loff_t *off){
	printk("SUB : Driver read function is called \n");
	copy_to_user(buf, (const char*)&subResult, 4);
	return 0;
}

static ssize_t writeFuncSUB(struct file *filp, const char __user *buf, size_t len, loff_t *off){
	printk("SUB : Driver write function is called \n");
	char kernelBuffer[20];
	int *a = NULL;
	int *b = NULL;
	printk("ADD Driver read function is called \n");
	copy_from_user(kernelBuffer, buf, len);
	a = (int*)kernelBuffer;
	b = (int*)&kernelBuffer[4];
	subResult = *a - *b;
	return 0;
}


static ssize_t readFuncMUL(struct file *filp, char __user *buf, size_t len, loff_t *off){
	printk("MUL :Driver read function is called \n");
	copy_to_user(buf, (const char*)&mulResult, 4);
	return 0;
}

static ssize_t writeFuncMUL(struct file *filp, const char __user *buf, size_t len, loff_t *off){
	printk("MUL : Driver write function is called \n");
	/*
	char kernelBuffer[20];
	int *a = NULL;
	int *b = NULL;
	printk("ADD Driver read function is called \n");
	copy_from_user(kernelBuffer, buf, len);
	a = (int*)kernelBuffer;
	b = (int*)&kernelBuffer[4];
	*/
	struct data{
		int a;
		int b;
	};
	struct data input;
	copy_from_user(&input, buf, len);
	mulResult = input.a * input.b;
	return 0;
}

static ssize_t readFuncDIV(struct file *filp, char __user *buf, size_t len, loff_t *off){
	printk("DIV : Driver read function is called \n");
	copy_to_user(buf, (const char*)&divResult, 4);
	return 0;
}

static ssize_t writeFuncDIV(struct file *filp, const char __user *buf, size_t len, loff_t *off){
	printk("DIV : Driver write function is called \n");
	char kernelBuffer[20];
	int *a = NULL;
	int *b = NULL;
	printk("DIV Driver read function is called \n");
	copy_from_user(kernelBuffer, buf, len);
	a = (int*)kernelBuffer;
	b = (int*)&kernelBuffer[4];
	divResult = *a / *b;
	return 0;
}

static int openFunc(struct inode *inode, struct file *file){
	printk("Driver open function is called \n");
	
	int minor;
	struct file_operations *newFops;
	newFops = file->f_op;
	devTemp = inode->i_rdev;
	minor = MINOR(devTemp);
	
	switch(minor){
		case 0 :
			newFops->read = readFuncADD;
			newFops->write = writeFuncADD;
			printk("ADD : new fops is %p and filep is %p", newFops, file);
			break;
		case 1 :
			newFops->read = readFuncSUB;
			newFops->write = writeFuncSUB;
			printk("SUB : new fops is %p and filep is %p", newFops, file);
			break;
		case 2 :
			newFops->read = readFuncMUL;
			newFops->write = writeFuncMUL;
			printk("MUL : new fops is %p and filep is %p", newFops, file);
			break;
		case 3 :
			newFops->read = readFuncDIV;
			newFops->write = writeFuncDIV;
			printk("DIV : new fops is %p and filep is %p", newFops, file);
			break;
	}
	return 0;
}

static int releaseFunc(struct inode *inode, struct file *file){
	printk("Driver release function is called \n");
	return 0;
}

static int helloInit(void){
	int i;
	int major;
	char *calcFunc[] = {"ADD", "SUB", "MUL", "DIV"};
	/*allocating Major number dynamically */
	alloc_chrdev_region(&dev, 0, 4, "calculatorDriver");
	printk("Kernel module is loaded \n");
	printk("Major = %d Minor = %d\n, dev value = %d\n", MAJOR(dev), MINOR(dev), dev);
	printk(KERN_INFO "%s : %s : %d\n", __FILE__, __func__, __LINE__);
	major = MAJOR(dev);
	/* device class creation to create device file inside /dev folder */
	devClass = class_create(THIS_MODULE, "calculatorClass");

	// creating multiple device files with same major no and different minor number.
	for(i = 0; i<4; i++){
		/* Creating cdev structure */
		cdev_init(&cdevStructure[i], &fops);
		devTemp = MKDEV(major, i);	//Major no from dev, Minor no from value of i
		
		/* Adding character device to the system */
		cdev_add(&cdevStructure[i], devTemp, 1);
	
		/* creating device file */
		device_create(devClass, NULL, devTemp, NULL, calcFunc[i]);
	}
	return 0;
}

static void helloExit(void){
	int i;
	int major;
	major = MAJOR(dev);
	// creating multiple device files with same major no and different minor number.
	for(i = 0; i<4; i++){
		devTemp = MKDEV(major, i);	//Major no from dev, Minor no from value of i
		cdev_del(&cdevStructure[i]);
		device_destroy(devClass, devTemp);
	}
	class_destroy(devClass);
	unregister_chrdev_region(dev, 1);
	printk("Kernel module is unloaded \n");
	printk(KERN_INFO "%s : %s : %d\n", __FILE__, __func__, __LINE__);
}

module_init(helloInit);
module_exit(helloExit);

