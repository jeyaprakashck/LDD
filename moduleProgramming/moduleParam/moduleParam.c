#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("Module Parameter example");
MODULE_LICENSE("GPLV2");

int value=0;
module_param(value, int, 0644);

static int  moduleParamInit(void){
	printk("The parameter value is %d\n", value);
	printk("%s : %s : %s \n", __FILE__, __func__, __LINE__);
	return 0;
}

static void moduleParamExit(void){
	printk("Module is unloaded\n");
}

module_init(moduleParamInit);
module_exit(moduleParamExit);
