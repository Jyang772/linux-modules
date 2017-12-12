	while(1) {
		usleep(10000);

		unsigned char buf[80];
		int rdlen;
		ioctl(fd, FIONREAD, &rdlen);
		if (rdlen > 0) {
			rdlen = read(fd, buf, rdlen);
		}
		if (rdlen > 0) {
            	...
