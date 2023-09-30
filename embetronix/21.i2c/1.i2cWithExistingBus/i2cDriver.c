#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#define I2C_BUS_AVAILABLE 1
#define SLAVE_DEVICE_ADDRESS 0x68
#define SLAVE_DEVICE_NAME "I2C_RTC"

static struct i2c_adapter *i2cAdapter = NULL;
static struct i2c_client *i2cClient = NULL;

/*
 * This function writes the data to the i2c client
 */
 
static int i2cWrite(unsigned char *buf, unsigned int len){
	/*
	 * start condition, slave address with RW bit, ACK/NACK and stop 
	 * conditon will be taken care internally
	 */
	int ret = i2c_master_send(i2cClient, buf, len);
	return ret;
}

static int i2cRead(unsigned char *buf, unsigned int len){
	int ret = i2c_master_recv(i2cClient, buf, len);
	return ret;
}

/*
 * This function will be called when the slave has ben found. 
 * This will be called only once we load the driver
 */

static int i2cProbe(struct i2c_client *client, const struct i2c_device_id *id){

	printk("i2c slave address : %x\n",client->addr);
	printk("Found i2c slave\n");
	return 0;
}

/*
 * This function will be called when the slave has been removed.
 * this will be called only once when we unload the driver
 */

static int i2cRemove(struct i2c_client *client){
	printk("i2c slave is removed\n");
	return 0;
}

/*
 * slave device ID structure
 */

static const struct i2c_device_id i2cDeviceID[] ={
       	{SLAVE_DEVICE_NAME, 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, i2cDeviceID);

/*
 * i2c driver which needed to be added to linux
 */
static struct i2c_driver i2cDriver = {
	.driver = {
		.name = SLAVE_DEVICE_NAME,
		.owner = THIS_MODULE,
	},
	.probe = i2cProbe,
	.remove = i2cRemove,
	.id_table = i2cDeviceID
	
};

/*
 * i2c board info
 */

static struct i2c_board_info i2cBoardInfo = {
	I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SLAVE_DEVICE_ADDRESS)
};

/*
 * module init function 
 */

int __init init_module(void){
	int ret = -1;
	int i;
	int addr = 0x00;
	i2cAdapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
	if(i2cAdapter != NULL){
		i2cClient = i2c_new_device(i2cAdapter, &i2cBoardInfo);
		if(i2cClient != NULL){
			i2c_add_driver(&i2cDriver);
			ret = 0;
		}	
		i2c_put_adapter(i2cAdapter);
	}
	printk("checking data from RTC\n");
	for(i=0; i<10;i++){
		//msleep(1000);
		printk("value from first address : %x\n", i2c_smbus_read_byte_data(i2cClient, addr++));
	}
	printk("I2C driver added\n");
	return ret;
}

void __exit cleanup_module(void){
	i2c_unregister_device(i2cClient);
	i2c_del_driver(&i2cDriver);
	printk("I2C driver removed\n");
}

MODULE_LICENSE("GPL");




