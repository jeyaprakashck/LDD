#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

int init_module(void){
	unsigned long j = jiffies + 10*HZ;			//jiffies variable will hold the timer interrupts (ticks)
	printk("Waiting for 10sec in polling methos\n");
	while(jiffies < j);
	printk("Sleep is over\n");
	return -1;		//not loading the module.
}


