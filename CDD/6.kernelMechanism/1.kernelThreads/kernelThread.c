#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/jiffies.h>


static struct task_struct *senderThread = NULL;

static int threadFunc(void *data){
	printk("name : %s, PID : %s, TGID : %d\n", current->comm, current->pid, current->tgid);
	while(1){
		printk("Inside kernel thread\n");
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(msecs_to_jiffies(3000));
		schedule();
		if(kthread_should_stop())
			break;
	}
	return 0;
}

int init_module(void){
	senderThread = kthread_run(threadFunc, NULL, "kernelThread");
	if(IS_ERR(senderThread)){
		printk("Error %ld creating thread\n", PTR_ERR(senderThread));
		return -1;
	}
	printk("Starting kernel thread in init module\n");
	return 0;
}

void cleanup_module(void){
	printk("unloading kernel thread module\n");
	kthread_stop(senderThread);
}
