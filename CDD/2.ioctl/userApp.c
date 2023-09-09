#include <stdio.h>
#include <fcntl.h>
#include <asm/ioctl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#define MAGIC_NUMBER 'a'

#define SET_BAUD_RATE  _IOWR(MAGIC_NUMBER, 8, int)
#define SET_NO_STOP_BITS _IOWR(MAGIC_NUMBER, 9, int)
#define SET_DIRECTION_WRITE _IOW (MAGIC_NUMBER, 10, int)



int fd;
char input[1024]="Hello from kernel";
char output[1024];

int main(){
	fd = open("/dev/ioctlExampleDriver", O_RDWR, 0777);
	//scanf("Enter the value to sent kernel : %s\n",&input);
	fgets(input, 1024, stdin);
	write(fd, input, 1024);
	read(fd, output, 1024);
	printf("the value from kernel is : %s\n", output);	
	ioctl(fd, SET_BAUD_RATE, 9600);
	ioctl(fd, SET_NO_STOP_BITS, 8);
	close(fd);
	return 0;
}
