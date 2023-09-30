#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>



char writeBuf[1024];
char readBuf[1024];
int fd;

int main(){
	fd = open("/dev/hellodev", O_RDWR);
	printf("Please enter the string\n");
	fgets(writeBuf, sizeof(writeBuf), stdin);
	printf("writing data to kernel...\n");
	write(fd, writeBuf, strlen(writeBuf)+1); 
	usleep(500);
	printf("Reading the data from kernel..");
	read(fd, readBuf, sizeof(readBuf));
	printf("The data from Kernel : %s\n", readBuf);
	return 0;
}

