#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>


MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("add function definition");
MODULE_LICENSE("GPLV2");


int add(int a , int b){
	return (a + b);
}

EXPORT_SYMBOL(add);

