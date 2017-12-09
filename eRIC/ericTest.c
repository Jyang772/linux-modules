#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>


int main()
{
	int fd;
	int opt;

	fd = open("/dev/ttyUSB0",O_RDWR);
	if(fd < 0) {
		perror("open:");
		return -1;
	}
	
	char byte;
	write(fd, "ER_CMD#T7",9);
	ssize_t size = read(fd, &byte, 1);
	printf("Read byte %c\n", byte);
	
	close(fd);
	return 0;
}
