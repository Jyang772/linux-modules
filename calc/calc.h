
#define DRIVER_NAME "calcboi"
#define MAJOR_DYNAMIC 0

struct calc
{
	int num1;
	int num2;
	int result;
};

#define SAMPLE_MAGIC 's'
#define SAMPLE_IOCTL_ADD	_IOWR(SAMPLE_MAGIC, 0x01, struct calc)
