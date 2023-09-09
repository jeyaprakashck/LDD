#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>


MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("Average function - exporting add symbol");
MODULE_LICENSE("GPLV2");


int add(int, int);

static int __init avgFuncModuleInit(void){
	printk("avg function module loaded\n");
	printk(" %s : %s : %s \n", __FILE__, __func__, __LINE__);
	printk("The average of 5 and 15 is %d\n", add(5, 15)/2);
	return 0;
}

static void __exit avgFuncModuleExit(void){
	printk("avg function module is exited\n");
}

module_init(avgFuncModuleInit);
module_exit(avgFuncModuleExit);

