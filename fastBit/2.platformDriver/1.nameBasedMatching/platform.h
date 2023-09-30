
/* platform's private data, which can be passed with platform device */
struct myDevicePlatformDataStruct{
	int size;
	int perm;
	const char *serialNumber;
};

#define RDWR	0x11
#define RDONLY	0x01
#define WRONLY	0x10
