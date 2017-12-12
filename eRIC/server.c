#include "readLine.h"

#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

struct pollfd fds[1];

int readReply(int, char*);

int set_interface_attribs(int fd, int speed)
{
	struct termios tty;

	if (tcgetattr(fd, &tty) < 0) {
		printf("Error from tcgetattr: %s\n", strerror(errno));
		return -1;
	}

	cfsetospeed(&tty, (speed_t)speed);
	cfsetispeed(&tty, (speed_t)speed);

	tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;         /* 8-bit characters */
	tty.c_cflag &= ~PARENB;     /* no parity bit */
	tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
	tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

	/* setup for non-canonical mode */
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	/* fetch bytes as they become available */
	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 0;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}


int main()
{
	char *portname = "/dev/ttyUSB0";
	int fd;
	int wlen;

	fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("Error opening %s: %s\n", portname, strerror(errno));
		return -1;
	}
	/*baudrate 115200, 8 bits, no parity, 1 stop bit */
	set_interface_attribs(fd, B19200);
	fds[0].fd = fd;
	fds[0].events = POLLIN;

	//TODO: WRITE BETTER DEBUG
	//REMOVE WHILE LOOP
	while(1) {
		//usleep(80000);
		unsigned char buf[80];
		int rdlen;

		    char ch = 0;
		    while(ch != '*') {
			    read(fd,&ch,1);
		    };
		    printf("NEXT\n");
		    if(ch == '*') {
			rdlen = readLine(fd,buf,sizeof(buf));
			printf("Command read: %s\n",buf);
		    }
		    else {
			  continue;
	    	    }
		//rdlen = read(fd, buf, sizeof(buf) - 1);
		//rdlen = readLine(fd, buf, sizeof(buf));
		//rdlen = readReply(fd, buf);
		//printf("Command read: %s\n",buf);
		
		if (rdlen > 0) {

			buf[rdlen] = 0;
			printf("Read %d: \"%s\"\n",rdlen, buf);

			if(strcmp("getTemp",buf) == 0) {
				printf("Inside\n");
				printf("getTemp\n");

				//Send appropriate command to local device
				//Read back output
				//Send output back to client
				wlen = write(fd, "ER_CMD#T7", 9);
				if (wlen != 9) {
					printf("Error from write: %d, %d\n", wlen, errno);
				}
				tcdrain(fd);
				usleep(800000);
				tcflush(fd, TCIOFLUSH);
				wlen = write(fd, "ACK", 3);
				if (wlen != 3) {
					printf("Error from write: %d, %d\n", wlen, errno);
				}

				usleep(50000); //Wait for device to send out temp.
				readLine(fd, buf, 80);
				printf("strlen: %d\n",strlen(buf));
				buf[strlen(buf)]='\n'; //add newline at end
				buf[strlen(buf)+1] = 0; //add null terminator at end
				printf("SENDING: %s",buf);
				printf("strlen: %d\n",strlen(buf));
				
				write(fd, buf, strlen(buf)+1);
				tcdrain(fd);
				usleep(50000);
			}
			else {
				printf("ERROR\n");
				usleep(4000);
				tcflush(fd, TCIOFLUSH);
			}
		}
	}
}


int readReply(int fd, char* buf) {
	int rdlen = 0;	
	unsigned char readIn[80];

	memset(buf, 0, 80);	

	int done = 0;
	int p = 0;

	while(!done) {
		rdlen = read(fd, readIn, sizeof(readIn)-1);
		if(rdlen > 0) {
			printf("Read_ %d: \"%s\"\n",rdlen, readIn);
			printf("%x\n",readIn[rdlen-1]);
			if(readIn[rdlen-1] == 0) {
				done = 1;
				printf("DONE");
			}
			strncpy(buf+p,readIn,rdlen);
			p += rdlen;
		} else if (rdlen < 0) {
			printf("Error from read: %d: %s\n", rdlen, strerror(errno));
		}
	}

	buf[strlen(buf)] = 0;
	printf("Sending: %s",buf);

	return strlen(buf);


}

