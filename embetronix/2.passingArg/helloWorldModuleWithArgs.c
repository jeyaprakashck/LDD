
/****************************************************************
 	CODE    : helloWorldWithArgs.c
	Author  : JP
	Details : Simple Linux Kernel Module with argumet passing
*****************************************************************/



#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

int value;
int arrValue[5];
char *name;
int valueCB;

module_param(value, int, S_IRUSR | S_IWUSR);
module_param(name, charp, S_IRUSR | S_IWUSR);
module_param_array(arrValue, int, NULL, S_IRUSR | S_IWUSR);


/************** module_param_cb()***************************/
int notification(const char *val, const struct kernel_param *kp){
	int res = param_set_int(val, kp);
	if(res == 0){
		printk(KERN_INFO "Callback function called ..\n");
		printk(KERN_INFO "New value of valeCB is %d\n", valueCB);
		return 0;
	}
	return -1;
}

const struct kernel_param_ops myParamOps={
	.set = &notification,
	.get = &param_get_int,
};

module_param_cb(valueCB, &myParamOps, &valueCB, S_IRUGO | S_IWUSR);


/*
 * init function
 */

static int __init helloWorldInit(void){
	int i;

	printk(KERN_INFO "Value = %d\n", value);
	printk(KERN_INFO "ValueCB = %d\n", valueCB);
	printk(KERN_INFO "Name = %s\n", name);
	for(i = 0; i<(sizeof(arrValue)/sizeof(int)); i++)
		printk(KERN_INFO "arrValue[%d] = %d\n", i, arrValue[i]);
	

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
