#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
	int fd;
	char buf;
	fd = open("/dev/interruptDeviceExample", O_RDWR);
	printf("waiting for 2 seconds before reading the file\n");
	sleep(2);
	printf("reading the file... press any key to exit from blocking\n");
	read(fd, &buf, 1024);
	close(fd);
	return 0;
}
