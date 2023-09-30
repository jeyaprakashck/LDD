#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/jiffies.h>


int init_module(void){
	printk("Waiting for 10sec in sleepy methos\n");
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(msecs_to_jiffies(10000));
	printk("Sleep is over\n");
	return -1;		//not loading the module.
}


