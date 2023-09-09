
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct add {
	int a;
	int b;
};
struct add addtest;

int main()
{	
	int fd, result;
	addtest.a = 35;
	addtest.b = 5;
	fd = open ("/dev/ADD", O_RDWR , 0777 ) ;
	if (fd <0 )
	{	
		perror ("open:%s") ;
		return -1 ;
	}
	if(write ( fd, &addtest, sizeof ( struct add ) ) < 0)
		perror("write") ;
	sleep(2);
	if(read ( fd, &result, 4 ) < 0)
		perror("read") ;
	printf ("\n Kernel Data: %d\n", result);
	close(fd);
	return 0 ;
}

