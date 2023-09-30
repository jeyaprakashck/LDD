
/************************************************************
 	CODE    : gpioExamp;e.c
	Author  : JP
	Details : LED operations (On/Off/Toggle) using gpio Device file 
		  using kthread.
************************************************************/



#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>		//kmalloc();
#include <linux/uaccess.h>	//copy to user, copy from user
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/string.h>	//strcmp function
#include <linux/timer.h>

#define GPIO_21_OUT 21
#define GPIO_25_IN  25

//creating the dev with custom major and minor number with dynamic method
dev_t dev = 0;
static struct class *devClass;
static struct cdev charDev;
char *kernelBuffer;

//timer parameters
static struct timer_list timer;
void timerFunc(struct timer_list *data){
	int gpioStatus = gpio_get_value(GPIO_21_OUT);
	gpio_set_value(GPIO_21_OUT, (0x01^gpioStatus));
	printk("TOGGLING the LED\n");
	mod_timer(&timer, msecs_to_jiffies(100));
}

/*
//GPIO Interrupts
extern unsigned long volatile jiffies;
unsigned long oldJiffies = 0;
unsigned int ledToggle = 0;
unsigned int gpioIRQNumber = 0;
static irqreturn_t gpioIRQHandler(int irq, void *devID){
	static unsigned long flag = 0;
	unsigned long dif = jiffies - oldJiffies;
	if(dif<20){
		return IRQ_HANDLED;
	}
	oldJiffies = jiffies;
	local_irq_save(flag);
	ledToggle = (0x01 ^ ledToggle);
	gpio_set_value(GPIO_21_OUT, ledToggle);
	printk("Interrupt occured: GPIO_21 %d\n", gpio_get_value(GPIO_21_OUT));
	local_irq_restore(flag);
	return IRQ_HANDLED;
}
*/


/*
 ** Function Prototype
 */
static int 	__init helloWorldInit(void);
static void	__exit helloWorldExit(void);
static int	helloDevOpen(struct inode *inode, struct file *file);
static int 	helloDevRelease(struct inode *inode, struct file *file);
static ssize_t 	helloDevRead(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t	helloDevWrite(struct file *file, const char __user *buf, size_t len, loff_t *offset);

static struct file_operations fops = {
	.owner 	= THIS_MODULE,
	.read 	= helloDevRead,
	.write	= helloDevWrite,
	.open	= helloDevOpen,
	.release= helloDevRelease,
};

/*
 * This function will be called whenever we open the device file
 */
static int helloDevOpen(struct inode *inode, struct file *file){
	pr_info("Driver open function is called\n");
	return 0;
}

/*
 * This function will be called whenever we close the device file
 */
static int  helloDevRelease(struct inode *inode, struct file *file){
	pr_info("Drvier close function is called\n");
	return 0;
}

/*
 * This function will be called whenever we write on device file
 */
static ssize_t helloDevWrite(struct file *file, const char __user *buf, size_t len, loff_t *offset){
	del_timer(&timer);
	pr_info("Driver write function is called\n");
	memset(kernelBuffer, 0, 1024);
	printk("string length :%d\n", len);
	if(copy_from_user(kernelBuffer, buf, len))
		pr_err("Error while writing..\n");
	printk("String from user : %s\n", kernelBuffer);	
	if(!strcmp(kernelBuffer, "LED_ON")){
		gpio_set_value(GPIO_21_OUT, 1);
		printk("LED_ON");
	}else if(!strcmp(kernelBuffer, "LED_OFF")){
		gpio_set_value(GPIO_21_OUT, 0);
		printk("LED_OFF");
	}else if(!strcmp(kernelBuffer, "LED_TOGGLE")){
		printk("LED_TOGGLE");
		mod_timer(&timer, msecs_to_jiffies(1));
	}else{
		printk("Error:(%s) please provide LED_ON or LED_OFF\n", kernelBuffer);
	}
	return len;
}

/*
 * This function will be called whenever we read from the device file
 */
static ssize_t helloDevRead(struct file *file, char __user *buf, size_t len, loff_t *offset){
       /*
	pr_info("Driver read function is called \n");
       uint8_t gpioState = 0;
       gpioState = gpio_get_value(GPIO_21_OUT);
       len = 1;
       if(copy_to_user(buf, &gpioState, len))
	       pr_err("Error while reading\n");
       pr_info("Data read successfully: %d\n", gpioState);
       */
       return 0;
}






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
	if(IS_ERR(device_create(devClass, NULL, dev, NULL, "gpioDev"))){
		pr_err("Cannot create the device file\n");
		goto r_devices;
	}
	
	/* Creating cdev structure */
	cdev_init(&charDev, &fops);

	/* Adding character device to the system */
	if((cdev_add(&charDev, dev, 1)) < 0){
		pr_err("Cannot add the device to the system \n");
		goto r_class;
	}
	
	/* Creating physical memory */
	if((kernelBuffer = kmalloc(1024, GFP_KERNEL)) == 0){
		pr_info("cannot allocate memory\n");
		goto r_devices;
	}

	/* gpio part */
	if(gpio_is_valid(GPIO_21_OUT) == 0){
		printk("GPIO 21 is not available\n");
		goto r_devices;
		goto r_class;
	}
	gpio_request(GPIO_21_OUT, "GPIO_21_OUT");
	gpio_direction_output(GPIO_21_OUT, 1);
	gpio_export(GPIO_21_OUT,1);

	//timer initialization
	timer_setup(&timer, timerFunc, 0);

	/*
	gpioIRQNumber = gpio_to_irq(GPIO_25_IN);
	printk("GPIO irq number is %d\n", gpioIRQNumber);
	if(request_irq(gpioIRQNumber, (void*)gpioIRQHandler, IRQF_TRIGGER_RISING, "GPIO25Interrupt", NULL)){
			printk("Cannot register IRQ\n");
			}
	*/

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
	gpio_unexport(GPIO_21_OUT);
	//free_irq(gpioIRQNumber, NULL);
	gpio_free(GPIO_21_OUT);
	kfree(kernelBuffer);
	device_destroy(devClass, dev);
	class_destroy(devClass);
	cdev_del(&charDev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Exiting \n");

}

module_init(helloWorldInit);
module_exit(helloWorldExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JP");
MODULE_VERSION("1:1.0");
MODULE_DESCRIPTION("GPIO example");
