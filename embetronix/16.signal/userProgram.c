#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>


#define SIGNO 44
#define REGISTER_CURRENT_TASK  _IOW('A', 1, int*)
int value = 0;
int number = 11;
int temp = 0;
int buf;

void sigEventHandler(int n, siginfo_t *info, void *unused){
	if(n == SIGNO){
		value = info->si_int;
		printf("received signal from kernel, and the received value %d\n", value);
	}
}

int main(){
	int fd;
	struct sigaction sig;
	/* custom signal handler */
	sigemptyset(&sig.sa_mask);
	sig.sa_flags	= (SA_SIGINFO | SA_RESETHAND);
	sig.sa_sigaction= sigEventHandler;
	sigaction(SIGNO, &sig, NULL);
	printf("installed signal handler for SIGNO : 40\n");
	printf("opening driver \n");
	fd = open("/dev/signaldev", O_RDWR);
	/*registering this task with kernel for signal */
	if(ioctl(fd, REGISTER_CURRENT_TASK, (int*) &number)){
		printf("ioctl failed\n");
		return -1;
	}

	while(!value){
		printf("waiting for signal ...\n");
		temp++;
		if(temp ==5){
			printf("reading the device file\n");
			read(fd, &buf, 4);
		}
		sleep(1);
	}
	printf("closing the driver\n");
	close(fd);

}
