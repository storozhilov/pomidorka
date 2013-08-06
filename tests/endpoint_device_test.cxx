#include <modbus.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define SERIAL_PORT "/dev/ttyUSB0"
#define DEVICE_ID 1
#define BAUD 38400
#define PARITY 'E'
#define DATA_BITS 8
#define STOP_BITS 1

#define GET_INPUT_BIT_CMD "gib"
#define GET_BIT_CMD "gb"
#define SET_BIT_CMD "sb"
#define GET_INPUT_REGISTER_CMD "gir"
#define GET_REGISTER_CMD "gr"
#define SET_REGISTER_CMD "sr"

#define REGISTERS_ADDRESS 0
#define REGISTERS_AMOUNT 5
#define WRITE_REGISTER_ADDRESS 1
#define WRITE_REGISTER_FIRST_VALUE 127
#define WRITE_REGISTER_SECOND_VALUE 255

void printUsage()
{
	printf("Usage endpoint_device_test <command> <arg_1>\nCommands:\n"
			"\tgib: \"Read input status\" (0x02),\n"
			"\tgb: \"Read coil status\" (0x01),\n"
			"\tsb: \"Force single coil\" (0x05),\n"
			"\tgir: \"Read input registers\" (0x04)\n"
			"\tgr: \"Read holding registers\" (0x04),\n"
			"\tsr: \"Preset single register\" (0x06)\n");
}

void test_read_registers(modbus_t * ctx)
{
	// Reading registers
	uint16_t registers[REGISTERS_AMOUNT];
	printf("Reading registers ... ");
	int registersFetched = modbus_read_registers(ctx, REGISTERS_ADDRESS, REGISTERS_AMOUNT, registers);
	if (registersFetched < 0) {
		fprintf(stderr, "Reading registers error: '%s'\n", modbus_strerror(errno));
		return;
	} else {
		printf("OK\n");
	}
	sleep(1);
	// Reading registers again
	printf("Reading registers again ... ");
	registersFetched = modbus_read_registers(ctx, REGISTERS_ADDRESS, REGISTERS_AMOUNT, registers);
	if (registersFetched < 0) {
		fprintf(stderr, "Reading registers error: '%s'\n", modbus_strerror(errno));
		return;
	} else {
		printf("OK\n");
	}
}

void test_write_register(modbus_t * ctx)
{
	// Writing register
	printf("Writing register ... ");
	if (modbus_write_register(ctx, WRITE_REGISTER_ADDRESS, WRITE_REGISTER_FIRST_VALUE) < 0) {
		fprintf(stderr, "Writing register error: '%s'\n", modbus_strerror(errno));
		return;
	} else {
		printf("OK\n");
	}
	sleep(1);
	// Writing register again
	printf("Writing register again ... ");
	if (modbus_write_register(ctx, WRITE_REGISTER_ADDRESS, WRITE_REGISTER_FIRST_VALUE) < 0) {
		fprintf(stderr, "Writing register error: '%s'\n", modbus_strerror(errno));
		return;
	} else {
		printf("OK\n");
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printUsage();
		return 1;
	}
	void (*test_func_ptr)(modbus_t *);
	if (!strcmp(argv[1], GET_REGISTER_CMD)) {
		test_func_ptr = test_read_registers;
	} else if (!strcmp(argv[1], SET_REGISTER_CMD)) {
		test_func_ptr = test_write_register;
	} else {
		fprintf(stderr, "Invalid command: '%s'\n", argv[1]);
		return 1;
	}
	modbus_t * ctx;
	ctx = modbus_new_rtu(SERIAL_PORT, BAUD, PARITY, DATA_BITS, STOP_BITS);
	if (!ctx) {
		fprintf(stderr, "MODBUS RTU context creation error: '%s'\n", modbus_strerror(errno));
		return 1;
	}
	modbus_set_debug(ctx, 1);
	if (modbus_set_slave(ctx, DEVICE_ID) < 0) {
		fprintf(stderr, "MODBUS slave setting error: '%s'\n", modbus_strerror(errno));
		modbus_free(ctx);
		return 1;
	}
	if (modbus_connect(ctx) < 0) {
		fprintf(stderr, "MODBUS connection error: '%s'\n", modbus_strerror(errno));
		modbus_free(ctx);
		return 1;
	}
	int serialMode = modbus_rtu_get_serial_mode(ctx);
	if (serialMode < 0) {
		fprintf(stderr, "Error fetching MODBUS serial mode: '%s'\n", modbus_strerror(errno));
		modbus_free(ctx);
		return 1;
	}
	printf("MODBUS serial mode is %s\n", serialMode == MODBUS_RTU_RS232 ? "MODBUS_RTU_RS232" : "MODBUS_RTU_RS485");
	//modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
	//modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_UP);
	test_func_ptr(ctx);
	modbus_free(ctx);
	return 0;
}
