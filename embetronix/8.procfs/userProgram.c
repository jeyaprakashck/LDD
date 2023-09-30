#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>


char writeBuf[1024];
char readBuf[1024];
//char value[1024] = NULL;
int fd;

#define WRITE_CMD	_IOW('A', 1, int*)
#define READ_CMD	_IOR('A', 2, int*)


int main(){
	fd = open("/proc/hello/helloProc", O_RDWR);
	if(fd<0){
		printf("error during device file open\n");
		return -1;
	}
	//printf("Please enter the string\n");
	//fgets(writeBuf, sizeof(writeBuf), stdin);
	//printf("writing data to kernel...\n");
	//write(fd, writeBuf, strlen(writeBuf)+1); 
	//usleep(500);
	//printf("Reading the data from kernel..");
	//read(fd, readBuf, sizeof(readBuf));
	//printf("The data from Kernel : %s\n", readBuf);
	
	printf("Please enter the string to Write into the proc file\n");
	fgets(writeBuf, sizeof(writeBuf), stdin);
	write(fd, &writeBuf, sizeof(writeBuf));
	printf("Data written into procfile\n");

	printf("Reading the data from procfile\n");
	read(fd, &readBuf, sizeof(readBuf));
	printf("The data read from procfile is %s\n", readBuf);	
	
	
	
	return 0;
}

