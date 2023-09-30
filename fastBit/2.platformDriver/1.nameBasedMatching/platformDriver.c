#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt, __func__

int checkPermission(int devPerm, int accessMode){
	if(devPerm == RDWR)
		return 0;
	
	/* ensures readonly access */
	if((devPerm == RDONLY) && ((accessMode & FMODE_READ) && !(accessMode & FMODE_WRITE)))
		return 0;
	
	/*ensures writeonly access */
	if((devPerm == WRONLY) && ((accessMode & FMODE_WRITE) && !(accessMode & FMODE_READ)))
		return 0;

	return -EPERM;
}

static int openFunc(struct inode *inode, struct file *file){

	return 0;
}

static int closeFunc(struct inode *inode, struct file *file){

	return 0;
}

static ssize_t readFunc(struct file *file, char __user *buf, size_t len, loff_t *offset){

	return 0;
}

static ssize_t writeFunc(struct file *file, const char __user *buf, size_t len, loff_t *offset){

	return 0;
}

static loff_t lseekFunc(struct file *file, loff_t offset, int whence){

	return 0;
}

/* file operations of the driver */
struct file_operations fops = {
	.open = openFunc,
	.release = closeFunc,
	.read = readFunc,
	.write = writeFunc,
	.llseek = lseekFunc,
	.owner = THIS_MODULE
};

/* platform driver data */

/* Device private data */
struct myDevicePrivateDataStruct{
	struct myDevicePlatformDataStruct myDevicePlatformData ;
	char *buffer;
	dev_t devNum;
	struct cdev cdev;
};

/* Driver Private data */
struct myDriverPrivateDataStruct {
	int totalDevices;
	dev_t devNumBase;
	struct class *myClass;
	struct device *myDevice;
}myDriverPrivateData;


/* this function gets called when matched platform device is found */
int myPlatformDriverProbe(struct platform_device *platformDevice){
	int ret;
	struct myDevicePlatformDataStruct *myDevicePlatformData;
	struct myDevicePrivateDataStruct *myDevicePrivateData;	
	
	pr_info("A device is detected\n");

	/* 1. Get the platform data */
	/*we can use dev_get_platdata macro to get the platformData(void return). This will be
	similar to (i.e platformDevice->dev.platform_data) */
	myDevicePlatformData = (struct myDevicePlatformDataStruct *)dev_get_platdata(&platformDevice->dev);
	if(!myDevicePlatformData){
		pr_info("No platform data available\n");
		ret = -EINVAL;
		return ret;
	}
	
	/* 2. Dynamically allocate memory for the device private data
	   here resource managment APIs are used (i.e devm)kzalloc) with this function kernel
	   automatically deallocate the memory when the particular device is removed(1st arg) */
	myDevicePrivateData = devm_kzalloc(&platformDevice->dev, sizeof(struct myDevicePrivateDataStruct), GFP_KERNEL);
	if(!myDevicePrivateData){
		pr_info("Cannot allocate memory \n");
		ret = -ENOMEM;
		return ret;
	}
	/* myDevicePrivateData is visible only in this probe function, So inorder to remove memory from 
	remove function, we need to pass this address along with the platform_device, this we can 
	archeive by linking this address to platform_device -> dev.driver_data (i.e using 
	dev_set_drvdata) */
	/* Save the device private data pointer in platform device structure */
	dev_set_drvdata(&platformDevice->dev, myDevicePrivateData); 	

	myDevicePrivateData->myDevicePlatformData.size = myDevicePlatformData->size;
	myDevicePrivateData->myDevicePlatformData.perm = myDevicePlatformData->perm;
	myDevicePrivateData->myDevicePlatformData.serialNumber = myDevicePlatformData->serialNumber;

	pr_info("Device Serial number = %s\n", myDevicePrivateData->myDevicePlatformData.serialNumber);
	pr_info("Device Size= %d\n", myDevicePrivateData->myDevicePlatformData.size);
	pr_info("Device permission = %d\n", myDevicePrivateData->myDevicePlatformData.perm);
	
	/* 3. Dynamically allocate memory for the platform device buffer using size information from the platform data. 
	   devm_kxalloc() (resource manage API) is used to free the memory automatically when device is removed */
	myDevicePrivateData->buffer = devm_kzalloc(&platformDevice->dev,myDevicePrivateData->myDevicePlatformData.size, GFP_KERNEL);
	if(!myDevicePrivateData){
		pr_info("Cannot allocate memory \n");
		ret = -ENOMEM;
		return ret;
	}
	
	/* 4. Get the device number */
	myDevicePrivateData->devNum = myDriverPrivateData.devNumBase + platformDevice->id;

	/* 5. Do cdev_init and cdev_add */
	cdev_init(&myDevicePrivateData->cdev, &fops);
	myDevicePrivateData->cdev.owner = THIS_MODULE;
	ret = cdev_add(&myDevicePrivateData->cdev, myDevicePrivateData->devNum, 1);
	if(ret < 0){
		pr_err("Cdev add failed\n");
		return ret;
	}
	
	/* 6. Create device file for the detected platform device */
	myDriverPrivateData.myDevice = device_create(myDriverPrivateData.myClass, NULL, myDevicePrivateData->devNum, NULL, "myPlatformDevice-%d", platformDevice->id);
	if(IS_ERR(myDriverPrivateData.myDevice)){
		pr_err("Device creation failed\n");
		ret = PTR_ERR(myDriverPrivateData.myDevice);
		cdev_del(&myDevicePrivateData->cdev);
		return ret;	
	}
	
	myDriverPrivateData.totalDevices++;

	pr_info(" A probe was successful \n");	
	return 0;

}

/* this function gets called when device is removed from the system */
int myPlatformDriverRemove(struct platform_device *platformDevice){
	/* getting details of myDevicePrivateDataStruct */
	struct myDevicePrivateDataStruct *myDevicePrivateData = dev_get_drvdata(&platformDevice->dev);
	/* 1. Remove a device that was created with device_create(); */
	device_destroy(myDriverPrivateData.myClass, myDevicePrivateData->devNum);

	/* 2. Remove a cdev entry from the system */
	cdev_del(&myDevicePrivateData->cdev);
	

	myDriverPrivateData.totalDevices--;
	pr_info(" A device is removed \n");
	return 0;

}

struct platform_driver myPlatformDriver = {
	.probe = myPlatformDriverProbe,
	.remove = myPlatformDriverRemove,
	/* struct device_driver driver; --> this member is to link the platformdevice */
	.driver = {
			/*if this name match with platformdevice, then probe
			  function of this driver will be called */
			.name = "pseudoCharDevice"
		},
};

#define MAX_DEVICES 10
static int __init platformDriverInit(void){
	int ret=0;
	/* 1. Dynamically allocate a device number for MAX_DEVICES */
	ret = alloc_chrdev_region(&myDriverPrivateData.devNumBase, 0, MAX_DEVICES, "myPlatformDevice");
	if(ret < 0){
		pr_err("alloc chardev region failes\n");
		return ret;
	}
	
	/* 2. Create device class under /sys/class */
	myDriverPrivateData.myClass = class_create(THIS_MODULE, "myPlatformClass");
	if(IS_ERR(myDriverPrivateData.myClass)){
		pr_err("Class creation failed\n");
		ret = PTR_ERR(myDriverPrivateData.myClass);
		unregister_chrdev_region(myDriverPrivateData.devNumBase, MAX_DEVICES);
		return ret;
	}
	
	/* 3. Register a platform driver */
	platform_driver_register(&myPlatformDriver);
	pr_info("my platform driver is loaded \n");
	return 0;
}

static void __exit platformDriverExit(void){
	/* 1. Unregister the platform driver */
	platform_driver_unregister(&myPlatformDriver);

	/* 2. Class destroy */
	class_destroy(myDriverPrivateData.myClass);

	/* 3. Unregister device number for MAX_DEVICES */
	unregister_chrdev_region(myDriverPrivateData.devNumBase, MAX_DEVICES);
	pr_info("my platform driver is unloaded \n");
}

module_init(platformDriverInit);
module_exit(platformDriverExit);

MODULE_LICENSE("GPL");
