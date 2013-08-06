#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <assert.h>

#define SERIAL_PORT "/dev/ttyUSB0"
#define DEVICE_ID 1
#define BAUDRATE 38400
#define READ_BUFFER_SIZE 1024

#define GET_INPUT_BIT_CMD "gib"
#define GET_BIT_CMD "gb"
#define SET_BIT_CMD "sb"
#define GET_INPUT_REGISTER_CMD "gir"
#define GET_REGISTER_CMD "gr"
#define SET_REGISTER_CMD "sr"

const char readRegistersCmd[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x05, 0x85, 0xC9};
const char writeRegisterCmd[] = {0x01, 0x06, 0x00, 0x01, 0x00, 0x7F, 0x99, 0xEA};

void printUsage()
{
	printf("Usage endpoint_device_raw_test <command> <arg_1>\nCommands:\n"
			"\tgib: \"Read input status\" (0x02),\n"
			"\tgb: \"Read coil status\" (0x01),\n"
			"\tsb: \"Force single coil\" (0x05),\n"
			"\tgir: \"Read input registers\" (0x04)\n"
			"\tgr: \"Read holding registers\" (0x04),\n"
			"\tsr: \"Preset single register\" (0x06)\n");
}

void dump_buffer(const char * buf, size_t buf_size)
{
	printf("[");
	for (size_t i = 0; i < buf_size; ++i) {
		if (i > 0) {
			printf(", ");
		}
		printf("0x%02X", (unsigned char) buf[i]);
	}
	printf("]");
}

int test_cmd(const char * cmd, size_t cmd_size)
{
	// See http://digilander.libero.it/robang/rubrica/serial.htm or libmodbus sources for info

	// Opening serial port
	int fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		fprintf(stderr, "Opening '%s' TTY device error: '%s'", SERIAL_PORT, strerror(errno));
		return 1;
	}

	// Fetching serial port options:
	struct termios options;
	tcgetattr(fd, &options);

	// Setting control options:
	// * CLOCAL: Local line - do not change "owner" of port
	// * CREAD: Enable receiver
	options.c_cflag |= (CLOCAL | CREAD);

	// Setting data bits:
	// * CSIZE: Bit mask for data bits
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	// Setting stop bits:
	// * CSTOPB: 2 stop bits (1 otherwise)
	options.c_cflag  &= ~CSTOPB;

	// Setting parity:
	// * PARENB: Enable parity bit
	// * PARODD: Use odd parity instead of even
	options.c_cflag |= PARENB;
	options.c_cflag &= ~PARODD;

	// Setting line options:
	// ICANON: Enable canonical input (else raw)
	// ECHO: Enable echoing of input characters
	// ECHOE: Echo erase character as BS-SP-BS
	// ISIG: Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	// Setting input options:
	// INPCK: Enable parity check
	// ISTRIP: Strip parity bits
	options.c_iflag |= (INPCK | ISTRIP);

	// Setting output options:
	// OPOST: Postprocess output (not set = raw output)
	options.c_oflag &= ~OPOST;

	// Applying new options to port
	if (tcsetattr(fd, TCSANOW, &options) < 0) {
		fprintf(stderr, "Applying new options to '%s' TTY device error: '%s'", SERIAL_PORT, strerror(errno));
		close(fd);
		return 1;
	}

	// Sending MODBUS command
	ssize_t bytes_sent = write(fd, cmd, cmd_size);
	if (bytes_sent < 0) {
		printf("Sending data to '%s' TTY device error: '%s'", SERIAL_PORT, strerror(errno));
		close(fd);
		return 1;
	}
	printf("%ld bytes have been sent to MODBUS device:\n", bytes_sent);
	dump_buffer(cmd, cmd_size);
	printf("\n");

	// Receiving a MODBUS response
	const struct timespec timeout = {
		/*.tv_sec =*/ 1,
		/*.tv_nsec =*/ 0
	};
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	int n = pselect(fd + 1, &readfds, NULL, NULL, &timeout, NULL);
	if (n < 0) {
		fprintf(stderr, "Awaiting for incoming data on '%s' TTY device error: '%s'", SERIAL_PORT, strerror(errno));
		close(fd);
		return 1;
	} else if (n == 0) {
		fprintf(stderr, "No incoming data on '%s' TTY device", SERIAL_PORT);
	} else {
		char buf[READ_BUFFER_SIZE];
		ssize_t bytes_received = read(fd, buf, READ_BUFFER_SIZE);
		printf("%ld bytes have been received from MODBUS device:\n", bytes_received);
		dump_buffer(buf, bytes_received);
		printf("\n");
	}
	close(fd);
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printUsage();
		return 1;
	}
	if (!strcmp(argv[1], GET_REGISTER_CMD)) {
		int res = test_cmd(readRegistersCmd, sizeof(readRegistersCmd));
		if (res != 0) {
			return res;
		}
		usleep(200);
		return test_cmd(readRegistersCmd, sizeof(readRegistersCmd));
	} else if (!strcmp(argv[1], SET_REGISTER_CMD)) {
		int res = test_cmd(writeRegisterCmd, sizeof(writeRegisterCmd));
		if (res != 0) {
			return res;
		}
		usleep(200);
		return test_cmd(writeRegisterCmd, sizeof(writeRegisterCmd));
	} else {
		fprintf(stderr, "Invalid command: '%s'\n", argv[1]);
		return 1;
	}
}
