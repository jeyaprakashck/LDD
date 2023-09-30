#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/slab.h>		

//Interrupt parameters
#define IRQ1	1
int interruptCount = 0;
irqreturn_t keyboardHandler(int irqno, void *data);
int id;

//Tasklet parameters (dynamic method)
struct tasklet_struct *tasklet = NULL;
void taskletFunc(unsigned long);	//bottom half function


//Function definitions
void taskletFunc(unsigned long data){
	printk("Inside the tasklet function(BottomHalf)\n");
	printk("Interrupt count = %d\n", interruptCount++);
}

irqreturn_t keyboardHandler(int irqno, void *data){
	printk("Inside the keyboard handler\n");
	printk("Calling bottom half ...\n");
	tasklet_schedule(tasklet);
	return IRQ_HANDLED;
}


int __init init_module(void){
	printk("Loading the module\n");
	if(request_irq(IRQ1, keyboardHandler, IRQF_SHARED, "keyboardInterrupt", &id)){
		printk("Error during interrupt request\n");
		return -1;
	}
	
	/* initialization of tasklet in dynamic method */
	tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
	tasklet_init(tasklet, taskletFunc, 0);
	printk("Module loaded and interrupt requested\n");
	return 0;	
}

void __exit cleanup_module(void){
	free_irq(IRQ1, &id);
	tasklet_kill(tasklet);
	kfree(tasklet);
	printk("interrupt module is unloaded\n");
}
		
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeyaprakash");
MODULE_DESCRIPTION("Keyboard handler example with Tasklet");
