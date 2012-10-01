//#include <isl/common.hxx>
//#include <isl/FileLogTarget.hxx>
#include <isl/ModbusEndpoint.hxx>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#define GET_INPUT_BIT_CMD "gib"
#define GET_BIT_CMD "gb"
#define SET_BIT_CMD "sb"
#define GET_INPUT_REGISTER_CMD "gir"
#define GET_REGISTER_CMD "gr"
#define SET_REGISTER_CMD "sr"

void printUsage()
{
	std::cout << "Usage mbclnt <tty_device> <endpoint_id> <command> <arg_1> [<arg_2> ...]" << std::endl <<
		"Commands:" << std::endl <<
		"gib: \"Read input status\" (0x02), arguments:" << std::endl <<
		"\t1) bitAddress: First bit address" << std::endl <<
		"\t2) bitsAmount: Bits amount (optional, default is 1)" << std::endl <<
		"gb: \"Read coil status\" (0x01), arguments:" << std::endl <<
		"\t1) bitAddress: First bit address" << std::endl <<
		"\t2) bitsAmount: Bits amount (optional, default is 1)" << std::endl <<
		"sb: \"Force single coil\" (0x05), arguments:" << std::endl <<
		"\t1) bitAddress: Bit address" << std::endl <<
		"\t2) bitValue: Bit value (0 is FALSE, any other value is TRUE)" << std::endl <<
		"gir: \"Read input registers\" (0x04), arguments:" << std::endl <<
		"\t1) registerAddress: First register address" << std::endl <<
		"\t2) registersAmount: Registers amount (optional, default is 1)" << std::endl <<
		"gr: \"Read holding registers\" (0x04), arguments:" << std::endl <<
		"\t1) registerAddress: First register address" << std::endl <<
		"\t2) registersAmount: Registers amount (optional, default is 1)" << std::endl <<
		"sr: \"Preset single register\" (0x06), arguments:" << std::endl <<
		"\t1) registerAddress: Register address" << std::endl <<
		"\t3) registerValue: Register value" << std::endl;
}

template <typename T> void dumpVector(const std::vector<T>& v)
{
	std::cout << '[';
	for (typename std::vector<T>::const_iterator i = v.begin(); i != v.end(); ++i) {
		if (i != v.begin()) {
			std::cout << ", ";
		}
		std::cout << static_cast<unsigned int>(*i);
	}
	std::cout << ']';
}

int main(int argc, char *argv[])
{
	//std::cout << "argc = " << argc << ", argv[0] = " << argv[0] << std::endl;
	if (argc < 5) {
		printUsage();
		exit(1);
	}
	std::stringstream oss;
	oss << argv[2];
	int endpointId;
	oss >> endpointId;
	if (oss.fail()) {
		std::cerr << "Invalid MODBUS endpoint ID: " << argv[2] << std::endl;
		exit(1);
	}
	//std::cout << "Device id is " << endpointId << ", oss.fail() is " << oss.fail() << std::endl;
	try {
		isl::ModbusEndpoint me(argv[1], endpointId, isl::ModbusEndpoint::Baud_57600, isl::ModbusEndpoint::EvenParity, isl::ModbusEndpoint::EightDataBits, isl::ModbusEndpoint::OneStopBit);
		me.open();
		if (!strcmp(argv[3], GET_INPUT_BIT_CMD)) {
			// Read input bits
			std::stringstream bitAddrSs;
			bitAddrSs << argv[4];
			int bitAddr;
			bitAddrSs >> bitAddr;
			if (bitAddrSs.fail()) {
				std::cerr << "Invalid input bit address (arg 1): " << argv[4] << std::endl;
				exit(1);
			}
			int bitsAmount = 1;
			if (argc > 5) {
				std::stringstream bitsAmountSs;
				bitsAmountSs << argv[5];
				bitsAmountSs >> bitsAmount;
				if (bitsAmountSs.fail()) {
					std::cerr << "Invalid input bits amount (arg 2): " << argv[5] << std::endl;
					exit(1);
				}
			}
			std::cout << "Sending \"read input status\" (0x02) MODBUS command to the endpoint ... ";
			std::vector<uint8_t> bits = me.readInputBits(bitAddr, bitsAmount);
			std::cout << "OK. Result is: ";
			dumpVector(bits);
			std::cout << std::endl;
		} else if (!strcmp(argv[3], GET_BIT_CMD)) {
			// Read bits
			std::stringstream bitAddrSs;
			bitAddrSs << argv[4];
			int bitAddr;
			bitAddrSs >> bitAddr;
			if (bitAddrSs.fail()) {
				std::cerr << "Invalid bit address (arg 1): " << argv[4] << std::endl;
				exit(1);
			}
			int bitsAmount = 1;
			if (argc > 5) {
				std::stringstream bitsAmountSs;
				bitsAmountSs << argv[5];
				bitsAmountSs >> bitsAmount;
				if (bitsAmountSs.fail()) {
					std::cerr << "Invalid bits amount (arg 2): " << argv[5] << std::endl;
					exit(1);
				}
			}
			std::cout << "Sending \"read coil status\" (0x01) MODBUS command to the endpoint ... ";
			std::vector<uint8_t> bits = me.readBits(bitAddr, bitsAmount);
			std::cout << "OK. Result is: ";
			dumpVector(bits);
			std::cout << std::endl;
		} else if (!strcmp(argv[3], SET_BIT_CMD)) {
			// Write bit
			if (argc < 6) {
				std::cerr << "No value for bit (arg 2)" << std::endl;
				exit(1);
			}
			std::stringstream bitAddrSs;
			bitAddrSs << argv[4];
			int bitAddr;
			bitAddrSs >> bitAddr;
			if (bitAddrSs.fail()) {
				std::cerr << "Invalid bit address (arg 1): " << argv[4] << std::endl;
				exit(1);
			}
			std::stringstream bitValueSs;
			bitValueSs << argv[5];
			int bitValue;
			bitValueSs >> bitValue;
			if (bitValueSs.fail()) {
				std::cerr << "Invalid bit value (arg 2): " << argv[5] << std::endl;
				exit(1);
			}
			std::cout << "Sending \"force single coil\" (0x05) MODBUS command to the endpoint ... ";
			me.writeBit(bitAddr, bitValue);
			std::cout << "OK." << std::endl;
		} else if (!strcmp(argv[3], GET_INPUT_REGISTER_CMD)) {
			// Read inpit registers
			std::stringstream registerAddrSs;
			registerAddrSs << argv[4];
			int registerAddr;
			registerAddrSs >> registerAddr;
			if (registerAddrSs.fail()) {
				std::cerr << "Invalid input register address (arg 1): " << argv[4] << std::endl;
				exit(1);
			}
			int registersAmount = 1;
			if (argc > 5) {
				std::stringstream registersAmountSs;
				registersAmountSs << argv[5];
				registersAmountSs >> registersAmount;
				if (registersAmountSs.fail()) {
					std::cerr << "Invalid input registers amount (arg 2): " << argv[5] << std::endl;
					exit(1);
				}
			}
			std::cout << "Sending \"read input registers\" (0x04) MODBUS command to the endpoint ... ";
			std::vector<uint16_t> registers = me.readInputRegisters(registerAddr, registersAmount);
			std::cout << "OK. Result is: ";
			dumpVector(registers);
			std::cout << std::endl;
		} else if (!strcmp(argv[3], GET_REGISTER_CMD)) {
			// Read registers
			std::stringstream registerAddrSs;
			registerAddrSs << argv[4];
			int registerAddr;
			registerAddrSs >> registerAddr;
			if (registerAddrSs.fail()) {
				std::cerr << "Invalid register address (arg 1): " << argv[4] << std::endl;
				exit(1);
			}
			int registersAmount = 1;
			if (argc > 5) {
				std::stringstream registersAmountSs;
				registersAmountSs << argv[5];
				registersAmountSs >> registersAmount;
				if (registersAmountSs.fail()) {
					std::cerr << "Invalid registers amount (arg 2): " << argv[5] << std::endl;
					exit(1);
				}
			}
			std::cout << "Sending \"read holding registers\" (0x03) MODBUS command to the endpoint ... ";
			std::vector<uint16_t> registers = me.readRegisters(registerAddr, registersAmount);
			std::cout << "OK. Result is: ";
			dumpVector(registers);
			std::cout << std::endl;
		} else if (!strcmp(argv[3], SET_REGISTER_CMD)) {
			// Write register
			if (argc < 6) {
				std::cerr << "No value for register (arg 2)" << std::endl;
				exit(1);
			}
			std::stringstream registerAddrSs;
			registerAddrSs << argv[4];
			int registerAddr;
			registerAddrSs >> registerAddr;
			if (registerAddrSs.fail()) {
				std::cerr << "Invalid register address (arg 1): " << argv[4] << std::endl;
				exit(1);
			}
			std::stringstream registerValueSs;
			registerValueSs << argv[5];
			int registerValue;
			registerValueSs >> registerValue;
			if (registerValueSs.fail()) {
				std::cerr << "Invalid register value (arg 2): " << argv[5] << std::endl;
				exit(1);
			}
			std::cout << "Sending \"preset single register\" (0x06) MODBUS command to the endpoint ... ";
			me.writeRegister(registerAddr, registerValue);
			std::cout << "OK." << std::endl;
		} else {
			std::cerr << "Unrecognized MODBUS-command: " << argv[3] << std::endl;
			exit(1);
		}
	} catch (std::exception& e) {
		std::cerr << "Executing modbus function error: " << e.what() << std::endl;
		exit(1);
	} catch (...) {
		std::cerr << "Executing modbus function unknown error" << std::endl;
		exit(1);
	}
}
