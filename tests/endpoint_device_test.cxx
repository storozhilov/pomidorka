#include <modbus.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define SERIAL_PORT "/dev/ttyUSB0"
#define DEVICE_ID 1
#define BAUD 38400
#define PARITY 'E'
#define DATA_BITS 8
#define STOP_BITS 1

#define REGISTERS_ADDRESS 0
#define REGISTERS_AMOUNT 5
#define WRITE_REGISTER_ADDRESS 1
#define WRITE_REGISTER_FIRST_VALUE 127
#define WRITE_REGISTER_SECOND_VALUE 255

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
	test_read_registers(ctx);
	sleep(1);
	test_write_register(ctx);
	modbus_free(ctx);
	return 0;
}
