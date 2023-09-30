#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");

static struct delayed_work dws;

static void queueFunc(struct work_struct *work){
	printk("Inside the queue function");
	printk("line2");
}

int __init init_module(void){
	INIT_DELAYED_WORK(&dws, queueFunc);
	schedule_delayed_work(&dws, msecs_to_jiffies(2000));
//	mod_delayed_work(system_wq, &queue, msecs_to_jiffies(100));

	printk("module inserted\n");
	
	return 0;
}

void cleanup_module(void){
	
	printk("removed\n");
}


