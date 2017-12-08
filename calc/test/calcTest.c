#include "../calc.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>


int main()
{
	int fd;
	int opt;
	struct calc pingu;

	fd = open("/dev/calcboi",O_RDWR);
	if(fd < 0) {
		perror("open:");
		return -1;
	}

	while(1) {
		printf("\n 1.Add\n2.Exit\nEnter:");
		scanf("%d",&opt);

		if(opt == 5)
			break;

		printf("Enter num1:");
		scanf("%d",&pingu.num1);

		printf("Enter num2:");
		scanf("%d",&pingu.num2);

		switch(opt)
		{
			case 1:
				ioctl(fd, SAMPLE_IOCTL_ADD, &pingu);
				break;
		}

		printf("Result: %d\n",pingu.result);
	}

	close(fd);
	return 0;
}
