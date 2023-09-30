/********************************************************
 * 	Details : This module will use manually created bus adapter(3) and
 * 	will do some basics operations. All operations will be excuted as 
 * 	per the function definitions from bus adapter module(e.g read/write)
 * ****************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>

#define I2C_BUS_AVAILABLE	3	//Newly created bus
#define SLAVE_DEVICE_NAME	"i2cRTC"
#define RTC_SLAVE_ADDRESS	0x50

static struct i2c_adapter *i2cAdapter;
static struct i2c_client *i2cClient;

char tempBuf[2] = {0x20, 0x99};
/*
 * This function is getting called when the slave has been found
 * This function will be called only once when we load the driver
 */
static int i2cProbeFunc(struct i2c_client *client, const struct i2c_device_id *id){
	printk("Inside the probe function\n");
	i2c_master_send(i2cClient, tempBuf, 2); 
	return 0;
}

/*
 * This function will be getting called when the slave has been removed
 * This will be called only once when we unload the driver
 */
static int i2cRemoveFunc(struct i2c_client *client){
	printk("Inside the [%s]\n", __func__);
	return 0;
}

/*
 * Slave device id structure
 */
static const struct i2c_device_id i2cDeviceIDs[] = {
	{SLAVE_DEVICE_NAME, 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, i2cDeviceIDs);

/*
 * i2c driver structure that has to be added to linux
 */
static struct i2c_driver i2cDriver = {
	.driver = {
		.name = SLAVE_DEVICE_NAME,
		.owner = THIS_MODULE
	},
	.probe = i2cProbeFunc,
	.remove = i2cRemoveFunc,
	.id_table = i2cDeviceIDs
};

/*
 * I2C board info structure
 */
static struct i2c_board_info i2cBoardInfo = {
	I2C_BOARD_INFO(SLAVE_DEVICE_NAME, RTC_SLAVE_ADDRESS)
};

static int __init myInit(void){
	i2cAdapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
	if(i2cAdapter != NULL){
		i2cClient = i2c_new_device(i2cAdapter, &i2cBoardInfo);
		if(i2cClient != NULL){
			i2c_add_driver(&i2cDriver);
		}
		i2c_put_adapter(i2cAdapter);
	}
	printk("Client driver added\n");
	return 0;
}

static void __exit myExit(void){
	i2c_unregister_device(i2cClient);
	i2c_del_driver(&i2cDriver);
	printk("Client driver removed\n");
}

module_init(myInit);
module_exit(myExit);

MODULE_LICENSE("GPL");		//This license need to be defined to access few symbols
