#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
int fd;
int gpioFD;
int gpioStatus=0;
char input[1024];


int main(){
	fd = open("/dev/gpioDev", O_RDWR);
	gpioFD = open("/sys/class/gpio/gpio21/value", O_RDONLY);
	while(1){
		
		printf("*****************************************\n \
			Enter LED_ON to turn on the LED \n\
			Enter LED_OFF to turn of the LED \n \
			Enter LED_TOGGLE to turn of the LED \n \
			****************************************\n");
		fgets(input, 1024, stdin);
		input[strlen(input)-1] ='\0';
		printf("The input is %s\n", input);
		if(write(fd, input, sizeof(input))<0){
			printf("Error during writing\n");
			return -1;
		}
		gpioStatus = 0;
		read(gpioFD, &gpioStatus, 4);
		printf("Wait... Current GPIO status : %d\n", gpioStatus);
		sleep(0.1);
	}
	close(fd);
	return 0;
}
		


