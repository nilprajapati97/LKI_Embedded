#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILE "/dev/Infinite_pmv"
#define MY_DATA "Hello Device File I am usr_app"

int main()
{
	int fd,ret=0;
	unsigned char usr_buff[100];
	memset(usr_buff,0,100);

	fd =  open(DEVICE_FILE, O_RDWR);
	if(fd<0)
	{
		perror("open");
	}	
	perror("open");

	ret = write(fd,MY_DATA,strlen(MY_DATA));
	if(ret<=0)
	{
		perror("write");
	}
	perror("write");
	
	ret =read(fd,usr_buff,strlen(MY_DATA));
	if(ret <=0)
	{
		perror("read");
	}
	perror("read");
	printf("data = %s\n",usr_buff);
	
	close(fd);
	perror("close");
	return 0;
}
