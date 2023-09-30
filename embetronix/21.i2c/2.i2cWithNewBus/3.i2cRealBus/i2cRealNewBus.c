/*************************************************
 * 	Details : This code explains creations of own simple
 * 	Bus drivers with proper read/write operations using GPIO
 * 	in bit banging method.
 *
 * ************************************************/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/delay.h>


#define ADAPTER_NAME 	"myi2cAdapter"

/******************* I2C related functions - Bit banging mehtod - START*****************/
#define SCL_GPIO	20	//This GPIO act as SCL
#define SDA_GPIO	21	//This GPIO act as SDA

#define I2C_DELAY usleep_range(5, 10)

/*
 * Function to read the SCL GPIO
 */
static bool myi2cReadSCL(void){
	gpio_direction_input(SCL_GPIO);
	return gpio_get_value(SCL_GPIO);
}

/*
 * Function to read the SDA GPIO
 */
static bool myi2cReadSDA(void){
	gpio_direction_input(SDA_GPIO);
	return gpio_get_value(SDA_GPIO);
}

/*
 * Function to clear the SCL GPIO
 */
static void  myi2cClearSCL(void){
	gpio_direction_output(SCL_GPIO, 0);
	gpio_set_value(SCL_GPIO, 0);
}

/*
 * Function to clear the SDA GPIO
 */
static void  myi2cClearSDA(void){
	gpio_direction_output(SDA_GPIO, 0);
	gpio_set_value(SDA_GPIO, 0);
}

/*
 * Function to set the SCL GPIO
 */
static void myi2cSetSCL(void){
	gpio_direction_output(SCL_GPIO, 1);
	gpio_set_value(SCL_GPIO, 1);
}

/*
 * Function to set the SDA GPIO
 */
static void myi2cSetSDA(void){
	gpio_direction_output(SDA_GPIO, 1);
	gpio_set_value(SDA_GPIO, 1);
}

/*
 * Function to initialize the GPIOs
 */
static int myi2cInit(void){
	//checking the SCL_GPIO and SCD_GPIO is valid or not
	gpio_free(SCL_GPIO);
	gpio_free(SDA_GPIO);
	if(gpio_is_valid(SCL_GPIO) == false){
		printk("SCL GPIO is not valid\n");
		return -1;
	}
	if(gpio_is_valid(SDA_GPIO) == false){
		printk("SDA GPIO is not valid\n");
		return -1;
	}

	//requesting SCL_GPIO and SDA_GPIO
	if(gpio_request(SCL_GPIO, "SCL_GPIO") < 0){
		printk("Error during SCL_GPIO request\n");
		return -1;
	}
	if(gpio_request(SDA_GPIO, "SDA_GPIO") < 0){
		printk("Error during SDA_GPIO request\n");
		gpio_free(SCL_GPIO);	//freeing already requested GPIO
		return -1;
	}

	/*
	 * configure the SCL_GPIO/SDA_GPIO as output, we will change the direction
	 * as per our need
	 */
	gpio_direction_output(SCL_GPIO, 1);
	gpio_direction_output(SDA_GPIO, 1);
	
	return 0;
}

/*
 * Function to deinitialize the GPIOs
 */
static void myi2cDeinit(void){
	gpio_free(SCL_GPIO);
	gpio_free(SDA_GPIO);
}

/*
 * Function to send the start condition
 * SDA High
 * SCL High
 * SDA Low (When SCL is high)
 */
static void myi2cStart(void){
	myi2cSetSDA();
	myi2cSetSCL();
	I2C_DELAY;
	myi2cClearSDA();
	I2C_DELAY;
	myi2cClearSCL();
	I2C_DELAY;
}

/*
 * Function to send the stop condtion
 * SDA LOW
 * SCL HIGH
 * SDA HIGH (when SCL is high)
 */
static void myi2cStop(void){
	myi2cClearSDA();
	I2C_DELAY;
	myi2cSetSCL();
	I2C_DELAY;
	myi2cSetSDA();
	I2C_DELAY;
	myi2cClearSCL();
}

/*
 * Function to read the SDA to get the status
* Returns 0 for NACK, return 1 for ACK
 */
static bool myi2cReadACK(void){
	//Reding ACK/NACK
	int ret = 1;
	I2C_DELAY;
	myi2cSetSCL();
	I2C_DELAY;
	if(myi2cReadSDA()){
		ret = 0;
	}
	myi2cClearSCL();
	return ret;
}

/*
 * Function to send the 7-bit address to the slave
 * Return 0 if success -1
 */
static int myi2cSendAddr(u8 addr, bool isRead){
	int i;
	u8 bit;
	int ret = -1;
	//writing 7 bit slave address
	for (i = 7; i > 0; i--){
		bit = (((addr) >> (i-1))&0x01);	//getting MSB value
		(bit) ? myi2cSetSDA() : myi2cClearSDA();	//writing MSB value
		I2C_DELAY;
		myi2cSetSCL();
		I2C_DELAY;
		myi2cClearSCL();
	}
	
	//writing Read/Write Bit (8th bit)
	(isRead) ? myi2cSetSDA() : myi2cClearSDA();
	I2C_DELAY;
	myi2cSetSCL();
	I2C_DELAY;
	myi2cClearSCL();
	I2C_DELAY;
	
	//checking ACK from client
	if(myi2cReadACK()){
		ret = 0;
	}

	return ret;
}

/*
 * Function to send the one byte to the slave
 * Return 0 on success, -1 on error
 */
static int myi2cSendByte(u8 byte){
	int ret = -1;
	u8 bit;
	int i;
	//Sending 8 bit data to client
	for(i = 0 ; i <= 7; i++){
		bit = ((byte >> (7-i))&(0x01));
		(bit) ? myi2cSetSDA() : myi2cClearSDA();
		I2C_DELAY;
		myi2cSetSCL();
		I2C_DELAY;
		myi2cClearSCL();
	}
	//checking ACK from client
	if(myi2cReadACK()){
		ret = 0;
	}
	return ret;
}

/*
 * Function to read the one byte from the slave
 */
static int myi2cReadByte(u8 *byte){
	//Not implemented
	return 0;
}

/*
 * Function to send the data byte by byte to slave
 */
static int myi2cSend(u8 slaveAddr, u8 *buf, u16 len){
	int i;
	//sending address to slave
	if(myi2cSendAddr(slaveAddr, false) < 0){
		printk("[File : %s] Error in [%s], Line NO [%d]\n", __FILE__,__func__, __LINE__);
		return -1;
	}
	//sending data byte by byte
	for(i = 0 ; i < len; i++){
		if(myi2cSendByte(buf[i]) < 0 ){
			printk("[File : %s] Error in [%s], Line NO [%d], [Data = 0x%02x]\n", __FILE__,  __func__, __LINE__, buf[i]);
			return -1;
		}
	}
	return 0;
}

/*
 * Function to read the data byte by byte from slave
 */
static int myi2cRead(u8 slaveAddr, u8 *buf, u16 len){
	//sending address to slave
	if(myi2cSendAddr(slaveAddr, true) < 0){
		printk("[File : %s] Error in [%s], Line NO [%d]\n", __FILE__,__func__, __LINE__);
		return -1;
	}
	//receiving data byte by byte
	for(i = 0 ; i < len; i++){
		if(myi2cReadByte(buf[i]) < 0 ){
			printk("[File : %s] Error in [%s], Line NO [%d], [Data = 0x%02x]\n", __FILE__,  __func__, __LINE__, buf[i]);
			return -1;
		}
	}
	return 0;
}

/********************* I2C related finction - bit banging method - END**************/


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
	if(myi2cInit() < 0){
		printk("[File : %s] myi2cInit : Error in [%s], LINE NO : [%d]\n",__FILE__,  __func__, __LINE__);
		return -1;
	}
	
	myi2cStart();
	
	for (i = 0; i<num; i++){
		int j;
		struct i2c_msg *msgTemp = &msg[i];
		if(myi2cSend(msgTemp->addr, msgTemp->buf, msgTemp->len)<0){
			printk("[File : %s] myi2cSend : Error in [%s], LINE NO : [%d]\n", __FILE__, __func__, __LINE__);
			return -1;
		}
		printk("[File : %s] [count = %d][%s] : [addr = 0x%x] [len = %d] [Data] = ", i, __FILE__,__func__, msgTemp->addr, msgTemp->len);
		for(j = 0; j < msgTemp->len; j++){
			printk("[0x%02x] ", msgTemp->buf[j]);
		}
	}

	myi2cStop();
	myi2cDeinit();
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

MODULE_LICENSE("GPL");
