
/************************************************************
 	CODE    : helloWorldWithMajorMinorNos.c
	Author  : JP
	Details : Simple Linux Kernel Module with Major & Minor number
************************************************************/



#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>

//creating the dev with custom major and minor number
dev_t dev = MKDEV(235, 0);


/*
 * init function
 */

static int __init helloWorldInit(void){
	register_chrdev_region(dev, 1, "testDev");
	printk(KERN_INFO "HelloWorld!!\n");
	printk(KERN_INFO "Module inserted\n");
	printk(KERN_INFO "Major no = %d  Minor No = %d \n", MAJOR(dev), MINOR(dev));
	return 0;
}

/*
 * exit function
 */

static void __exit helloWorldExit(void){
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Exiting \n");

}

module_init(helloWorldInit);
module_exit(helloWorldExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JP");
MODULE_VERSION("1:1.0");
MODULE_DESCRIPTION("Simple Hello World Kernel module");
