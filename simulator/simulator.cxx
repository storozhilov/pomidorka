#include <modbus/modbus.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
//#include <signal.h>
#include <iostream>
#include <iomanip>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "Usage: receiver <TTY_DEVICE>" << std::endl;
	}
	modbus_t *ctx;
	ctx = modbus_new_rtu(argv[1], 115200, 'E', 8, 1);
	if (ctx == NULL) {
		std::cerr << "Error libmodbus context creation: \"" << modbus_strerror(errno) << '"' << std::endl;
		return -1;
	}
	if (modbus_connect(ctx) == -1) {
		std::cerr << "Establishing MODBUS connection error: \"" << modbus_strerror(errno) << '"' << std::endl;
		modbus_free(ctx);
		return -1;
	}
	modbus_set_slave(ctx, 1);
	for (;;) {
		std::cout << "loop" << std::endl;
		uint8_t query[MODBUS_RTU_MAX_ADU_LENGTH];
		int rc = modbus_receive(ctx, query);
		if (rc < 0) {
			std::cerr << "Error receiving MODBUS request: \"" << modbus_strerror(errno) << '"' << std::endl;
			continue;
		} else if (rc == 0) {
			std::cout << "MODBUS request has been ignored" << std::endl;
			continue;
		}
		std::cout << rc << std::endl;
		std::cout << query << std::endl;
		std::cout << std::dec << std::noshowbase << rc << " bytes have been read from the serial port:" << std::endl;
		for (int i = 0; i < rc; ++i) {
			std::cout << (i == 0 ? "" : " ") << std::hex << std::setw(2) << std::setfill('0') <<
				std::showbase << static_cast<int>(query[i]);
		}
		std::cout << std::endl;
		//sleep(1);
		/*if (rc >= 0) {
			modbus_reply(ctx, query, rc, mb_mapping);
		} else {
			// Connection closed by the client or server
			break;
		}*/
	}
}
