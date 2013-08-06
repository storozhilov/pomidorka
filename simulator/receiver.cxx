#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/select.h>
#include <assert.h>
#include <iostream>
#include <iomanip>

#define BUFFER_SIZE 4096
//#define IS_NON_BLOCKING 1
#define IS_NON_BLOCKING 0
#define BAUDRATE B115200

char buffer[BUFFER_SIZE];

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "Usage: receiver <TTY_DEVICE>" << std::endl;
	}
	//int fd = open(argv[1], IS_NON_BLOCKING ? O_RDWR | O_NOCTTY | O_NDELAY : O_RDWR | O_NOCTTY);
	int fd = open(argv[1], O_RDWR | O_NOCTTY | (IS_NON_BLOCKING ? O_NDELAY : 0));
	//int fd = open(argv[1], O_RDWR | O_NOCTTY);
	if (fd < 0) {
		std::cerr << "Open \"" << argv[1] << "\" TTY device error: " << strerror(errno) << std::endl;
		return 1;
	}
	/*if (fcntl(fd, F_SETFL, 0) < 0) {
		std::cerr << "fcntl() call error: " << strerror(errno) << std::endl;
		return 1;
	}
	if (fcntl(fd, F_SETFL, FNDELAY) < 0) {
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
	while (1) {
		const struct timespec timeout = {
			/*.tv_sec =*/ 1,
			/*.tv_nsec =*/ 0
		};
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		int n = pselect(fd + 1, &readfds, NULL, NULL, &timeout, NULL);
		if (n < 0) {
			std::cerr << "Error awaiting data on serial port: " << strerror(errno) << std::endl;
		} else if (n == 0) {
			std::cout << "No data on serial port" << std::endl;
			continue;
		}
		ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 0) {
			if (IS_NON_BLOCKING && errno == EAGAIN) {
//				std::cout << "No data on serial port" << std::endl;
			} else {
				std::cerr << "Error reading data from serial port: " << strerror(errno) << std::endl;
				return 1;
			}
		} else {
			//std::cout.write(buffer, bytes_read) << std::endl;
			std::cout << std::dec << std::noshowbase << bytes_read << " bytes have been read from the serial port:" << std::endl;
			for (int i = 0; i < bytes_read; ++i) {
				std::cout << (i == 0 ? "" : " ") << std::hex << std::setw(2) << std::setfill('0') <<
					std::showbase << static_cast<int>(buffer[i]);
			}
			std::cout << std::endl;
		}
	}
	close(fd);
	return 0;
}
