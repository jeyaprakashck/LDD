/*************************************************
 * 	Details : This code explains creations of own simple
 * 	Bus drivers (Without proper read/write operations
 *
 * ************************************************/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>


#define ADAPTER_NAME 	"myi2cAdapter"

/*
 *  This function is used to get the functionalities that are supported
 *  by this bus driver
 */
static u32 myi2cFunc(struct i2c_adapter *adapter){
	return (I2C_FUNC_I2C		|
		I2C_FUNC_SMBUS_QUICK	|
		I2C_FUNC_SMBUS_BYTE	|
		I2C_FUNC_SMBUS_BYTE_DATA|
		I2C_FUNC_SMBUS_WORD_DATA|
		I2C_FUNC_SMBUS_BLOCK_DATA);
}

/*
 * This function will be called whenever you call i2c read, write APIs like
 * i2c_master_send(), i2c_master_recv() etc.
 */
static s32 myi2c_xfer(struct i2c_adapter *adapter, struct i2c_msg *msg, int num){
	int i;
	for (i = 0; i<num; i++){
		int j;
		struct i2c_msg *msgTemp = &msg[i];
		printk("Message received \n");
		printk("[count = %d][%s] : [addr = 0x%x] [len = %d] [Data] = ", i, __func__, msgTemp->addr, msgTemp->len);
		for(j = 0; j < msgTemp->len; j++){
			printk("[0x%02x] ", msgTemp->buf[j]);
		}
	}
	return 0;
}

/*
 * This function will be called when you call SMBUS read, write APIs
 */
static s32 mysmbus_xfer(struct i2c_adapter *adapter, 
			u16 addr, 
			unsigned short flags, 
			char read_write, 
			u8 command,
			int size,
			union i2c_smbus_data *data){
	printk("Inside the %s\n", __func__);
	return 0;
}

/*
 * I2C algorithm structure
 */
static struct i2c_algorithm myi2cAlgorithm = {
	.smbus_xfer = mysmbus_xfer,
	.master_xfer = myi2c_xfer,
	.functionality = myi2cFunc
};

/*
 * I2c adapter structure
 */
static struct i2c_adapter myi2cAdapter = {
	.owner = THIS_MODULE,
	.class = I2C_CLASS_HWMON,
	.algo  = &myi2cAlgorithm,
	.name  = ADAPTER_NAME,
};

static int __init myInit(void){
	int ret = -1;
	ret = i2c_add_adapter(&myi2cAdapter);
	printk("Bus driver added with name [%s]\n",ADAPTER_NAME);
	return ret;
}

static void __exit myExit(void){
	i2c_del_adapter(&myi2cAdapter);
	printk("Bus driver removed successfully [%s]\n", ADAPTER_NAME);
}
module_init(myInit);
module_exit(myExit);
