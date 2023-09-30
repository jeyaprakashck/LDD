#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

//miscdevice variables and functions

static int miscOpen(struct inode *inode, struct file *file);
static int miscClose(struct inode *inode, struct file *file);
static ssize_t miscRead(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t miscWrite(struct file *file, const char __user *buf, size_t len, loff_t *off);

struct file_operations fops = {
	.owner	= THIS_MODULE,
	.open 	= miscOpen,
	.release= miscClose,
	.read	= miscRead,
	.write 	= miscWrite
};


static int miscOpen(struct inode *inode, struct file *file){
	printk("misc driver is opened\n");
	return 0;
}
static int miscClose(struct inode *inode, struct file *file){
	printk("misc driver is closed\n");
	return 0;
}
static ssize_t miscRead(struct file *file, char __user *buf, size_t len, loff_t *off){
	printk("misc read function is called\n");
	return 0;
}
static ssize_t miscWrite(struct file *file, const char __user *buf, size_t len, loff_t *off){
	printk("misc write function is called \n");
	return len;
}

struct miscdevice miscDevice = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "miscDeviceExample",
	.fops	= &fops,
};


/*
 * misc init function 
 */
static int __init misc_init(void){
	misc_register(&miscDevice);
	printk("misc device is registered\n");
	return 0;
}

/*
 * misc exit function
 */
static void __exit misc_exit(void){
	misc_deregister(&miscDevice);
	printk("misc device is unregistered\n");
}

module_init(misc_init);
module_exit(misc_exit);

MODULE_LICENSE("GPL");
