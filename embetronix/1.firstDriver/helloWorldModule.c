
/************************************************************
 	CODE    : helloWorld.c
	Author  : JP
	Details : Simple Linux Kernel Module
************************************************************/



#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

/*
 * init function
 */

static int __init helloWorldInit(void){
	
	printk(KERN_INFO "HelloWorld!!\n");
	printk(KERN_INFO "Module inserted\n");
	return 0;
}

/*
 * exit function
 */

static void __exit helloWorldExit(void){
	
	printk(KERN_INFO "Exiting \n");

}

module_init(helloWorldInit);
module_exit(helloWorldExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JP");
MODULE_VERSION("1:1.0");
MODULE_DESCRIPTION("Simple Hello World Kernel module");
