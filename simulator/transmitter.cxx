#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <assert.h>
#include <iostream>

#define BAUDRATE B115200

const char *data_to_send = "Some data to send";

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "Usage: transmitter <TTY_DEVICE>" << std::endl;
	}
	int fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
	//int fd = open(argv[1], O_RDWR | O_NOCTTY);
	if (fd < 0) {
		std::cerr << "Open \"" << argv[1] << "\" TTY device error: " << strerror(errno) << std::endl;
		return 1;
	}
	/*if (fcntl(fd, F_SETFL, 0) < 0) {
		std::cerr << "fcntl() call error: " << strerror(errno) << std::endl;
		return 1;
	}*/
	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag |= (CLOCAL | CREAD);
	cfsetispeed(&options, BAUDRATE);
	cfsetospeed(&options, BAUDRATE);
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cflag |= PARENB;
	options.c_cflag &= ~PARODD;
	options.c_iflag |= (INPCK | ISTRIP);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;
	tcsetattr(fd, TCSANOW, &options);
	size_t data_length = strlen(data_to_send);
	while (1) {
		size_t total_bytes_written = 0;
		while (total_bytes_written < data_length) {
			ssize_t bytes_written = write(fd, data_to_send + total_bytes_written, data_length - total_bytes_written);
			if (bytes_written < 0) {
				std::cerr << "Error writing to serial port: " << strerror(errno) << std::endl;
				return 1;
			}
			//std::cout << bytes_written << " bytes have been written to TTY" << std::endl;
			total_bytes_written += bytes_written;
		}
		std::cout << "All data has been written to serial port" << std::endl;
		sleep(1);
	}
	close(fd);
	return 0;
}
