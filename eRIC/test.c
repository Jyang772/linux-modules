#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

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

    /* simple output */
    wlen = write(fd, "ER_CMD#T7", 9);
    if (wlen != 9) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }

    tcdrain(fd);
    usleep(100000);
    usleep(2); //required to make flush work, for some reason
    tcflush(fd,TCIOFLUSH);
	
    wlen = write(fd, "ACK", 3);
    if (wlen != 3) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }

    tcdrain(fd);
   
    usleep(100000); 

    //TODO: WRITE BETTER DEBUG
    //REMOVE WHILE LOOP
    while(1) {
    /* simple output */
    wlen = write(fd, "ER_CMD#T7", 9);
    if (wlen != 9) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }

    tcdrain(fd);
    usleep(100000);
    usleep(2); //required to make flush work, for some reason
    tcflush(fd,TCIOFLUSH);
	
    wlen = write(fd, "ACK", 3);
    if (wlen != 3) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }

    tcdrain(fd);
   
    usleep(100000);

    /* simple noncanonical input */
    //do {
        unsigned char buf[80];
        int rdlen;

        rdlen = read(fd, buf, sizeof(buf) - 1);
        if (rdlen > 0) {
#ifdef DISPLAY_STRING
            buf[rdlen] = 0;
            printf("Read %d: \"%s\"\n", rdlen, buf);
	    buf[rdlen] = '\n';
	    write(fd,buf,rdlen+1);
	    tcdrain(fd);
	    usleep(50000);
#else /* display hex */
            unsigned char   *p;
            printf("Read %d:", rdlen);
            for (p = buf; rdlen-- > 0; p++)
                printf(" 0x%x", *p);
            printf("\n");
#endif
        } else if (rdlen < 0) {
            printf("Error from read: %d: %s\n", rdlen, strerror(errno));
        }
        /* repeat read to get full message */
    //} while (1);
    }
}
