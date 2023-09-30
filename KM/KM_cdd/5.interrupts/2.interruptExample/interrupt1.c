#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/io.h>

int id;
int count = 0;

//Tasklet example

struct tasklet_struct task;
void bottomHalfTasklet(unsigned long);

void bottomHalfTasklet(unsigned long data){
	printk("Tasklet is scheduled\n");
}

irqreturn_t keyboardHandler(int irq, void *id){
	printk("Inside the handler\n");
	count++;
	printk("The count is %d \n", count);
	tasklet_schedule(&task);
	printk("Tasklet is calling %d\n", count);
	return IRQ_HANDLED;
}

int init_module(void){
	int rv;
	task.func = &bottomHalfTasklet;
	rv = request_irq(1, keyboardHandler, IRQF_SHARED, "KeyboardInterruptExample", &id);
	printk("rv : %d, id : %d\n", rv, id);
	if(rv<0){
		printk("<1> cant get interrupt\n");
		return -1;
	}
	return 0;
}

void cleanup_module(void){
	printk("interrupt module in unloaded\n");
	free_irq(1, &id);
}


