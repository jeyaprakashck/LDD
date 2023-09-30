#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>

MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("Hello Module program");
MODULE_LICENSE("GPLV2");


static int helloInit(void){
	printk("Kernel module is loaded \n");
	printk(KERN_INFO "PID : %d Process Name : %s\n", current->pid, current->comm);
	printk(KERN_INFO "%s : %s : %s\n", __FILE__, __func__, __LINE__);
	return 0;
}

static void helloExit(void){
	printk("Kernel module is unloaded \n");
	printk(KERN_INFO "%s : %s : %s\n", __FILE__, __func__, __LINE__);
}

module_init(helloInit);
module_exit(helloExit);

