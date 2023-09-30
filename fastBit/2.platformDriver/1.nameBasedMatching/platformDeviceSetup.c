#include <linux/module.h>
#include <linux/platform_device.h>
#include "platform.h"

/* create 2 platform data (declared in platform.h) 
   This platform specific data we can pass to platform_device via 
   struct device -> *platform_data 					*/
struct myDevicePlatformDataStruct myDevData[2] = {
	[0] = { .size = 512, .perm = RDWR, .serialNumber = "myDev1"},
	[1] = { .size = 1024, .perm = RDWR, .serialNumber = "myDev2"}
};
/* This callback function is to free the device after all reference have gone away */
void platformDeviceRelease(struct device *dev){
	pr_info("Inside the platform Device release function \n");
}
/* create 2 platform devices */

struct platform_device platformDev1 = {
	.name = "pseudoCharDevice",
	.id = 0,
	.dev = {
		.platform_data = &myDevData[0],
		.release = platformDeviceRelease,
	}
};

struct platform_device platformDev2 = {
	.name = "pseudoCharDevice",
	.id = 1,
	.dev = {
		.platform_data = &myDevData[1],
		.release = platformDeviceRelease
	}
}; 


static int __init platformInit(void){
	/* register platform devices */
	platform_device_register(&platformDev1);
	platform_device_register(&platformDev2);
	pr_info("Platform Device setup module is loaded \n");
	return 0;
}

static void __exit platformExit(void){
	/*removing platform devices */
	platform_device_unregister(&platformDev1);
	platform_device_unregister(&platformDev2);
	pr_info("Platform device setup module is unloaded\n");
}

module_init(platformInit);
module_exit(platformExit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Registration of two platform devices");
