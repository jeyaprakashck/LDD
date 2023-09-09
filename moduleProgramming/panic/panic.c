#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>

MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("Panic Module program");
MODULE_LICENSE("GPLV2");

int *ptr = NULL;
int a;
static int panicInit(void){
	a = *ptr;
	printk("Kernel module is loaded \n");
	printk(KERN_INFO "PID : %d Process Name : %s\n", current->pid, current->comm);
	printk(KERN_INFO "%s : %s : %s\n", __FILE__, __func__, __LINE__);
	return 0;
}

static void panicExit(void){
	printk("Kernel module is unloaded \n");
	printk(KERN_INFO "%s : %s : %s\n", __FILE__, __func__, __LINE__);
}

module_init(panicInit);
module_exit(panicExit);

