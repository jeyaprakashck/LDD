#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("Hello Module program");
MODULE_LICENSE("GPLV2");

dev_t dev = MKDEV(230, 0);


static int helloInit(void){
	register_chrdev_region(dev, 4, "majorNoExampletest");
	printk("Kernel module is loaded \n");
	printk("Major = %d Minor = %d\n, dev value = %d\n", MAJOR(dev), MINOR(dev), dev);
	printk(KERN_INFO "%s : %s : %d\n", __FILE__, __func__, __LINE__);
	return 0;
}

static void helloExit(void){
	unregister_chrdev_region(dev, 4);
	printk("Kernel module is unloaded \n");
	printk(KERN_INFO "%s : %s : %d\n", __FILE__, __func__, __LINE__);
}

module_init(helloInit);
module_exit(helloExit);

