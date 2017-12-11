#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

void readReply(int, char*);

ssize_t
readLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */

        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;

        } else {                        /* 'numRead' must be 1 if we get here */
            if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }

            if (ch == 0)
                break;
        }
    }

    *buf = '\0';
    return totRead;
}

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

	//TODO: WRITE BETTER DEBUG
	//REMOVE WHILE LOOP
	while(1) {
		unsigned char buf[80];
		int rdlen;

		rdlen = read(fd, buf, sizeof(buf) - 1);
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
				buf[strlen(buf)] = '\n';
				write(fd, buf, strlen(buf));
				tcdrain(fd);
				usleep(50000);
			}
		}
	}
}


void readReply(int fd, char* buf) {


	
	int rdlen = 0;	
	unsigned char readIn[80];

	memset(buf, 0, 80);	

	int done = 0;
	int p = 0;

	while(!done) {
		rdlen = read(fd, readIn, sizeof(readIn)-1);
		if(rdlen > 0) {
			printf("Read_ %d: \"%s\"\n",rdlen, readIn);
			printf("%x\n",readIn[rdlen]);
			if(readIn[rdlen] == 0) {
				done = 1;
				printf("DONE");
			}
			strncpy(buf+p,readIn,rdlen);
			p += rdlen;
		} else if (rdlen < 0) {
			printf("Error from read: %d: %s\n", rdlen, strerror(errno));
		}
	}

	buf[strlen(buf)] = '\n';
	printf("Sending: %s",buf);
	return;


}
