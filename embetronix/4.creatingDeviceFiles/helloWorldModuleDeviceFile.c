
/************************************************************
 	CODE    : helloWorldModuleDeviceFile.c
	Author  : JP
	Details : Simple Linux Kernel Module with creation of device file
************************************************************/



#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/err.h>
#include <linux/device.h>

//creating the dev with custom major and minor number with dynamic method
dev_t dev = 0;
static struct class *devClass;


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
	unregister_chrdev_region(dev, 1);
	class_destroy(devClass);
	device_destroy(devClass, dev);
	printk(KERN_INFO "Exiting \n");

}

module_init(helloWorldInit);
module_exit(helloWorldExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JP");
MODULE_VERSION("1:1.0");
MODULE_DESCRIPTION("Simple Hello World Kernel module");
