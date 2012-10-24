#ifndef GREEN_HOUSE_SERVER__HXX
#define GREEN_HOUSE_SERVER__HXX 1

#include <isl/Server.hxx>
#include <isl/LogMessage.hxx>
#include <isl/ModbusEndpoint.hxx>
#include "LightingController.hxx"

#define ENDPOINT_DEVICE "/dev/ttyUSB0"
#define ENDPOINT_ID 1
#define ENDPOINT_BAUD isl::ModbusEndpoint::Baud_57600
#define ENDPOINT_PARITY isl::ModbusEndpoint::EvenParity
#define ENDPOINT_DATA_BITS isl::ModbusEndpoint::EightDataBits
#define ENDPOINT_STOP_BITS isl::ModbusEndpoint::OneStopBit
//#define LIGHTING_RELAY_STATE_BIT_ADDR 1
#define LIGHTING_RELAY_STATE_BIT_ADDR 2
//#define LIGHTING_RELAY_FEEDBACK_BIT_ADDR 2
#define LIGHTING_RELAY_FEEDBACK_BIT_ADDR 0
#define LIGHTING_SENSOR_REGISTER_ADDR 4
#define TIMER_CLOCK_TIMEOUT_SECONDS 1
#define TIMER_CLOCK_TIMEOUT_NANOSECONDS 0
#define LIGHTING_CONTROLLER_CLOCK_TIMEOUT_SECONDS 5
#define LIGHTING_CONTROLLER_CLOCK_TIMEOUT_NANOSECONDS 0

class GreenHouseServer : public isl::Server
{
public:
	GreenHouseServer(int argc, char * argv[]) :
		isl::Server(argc, argv),
		_modbusEndpoint(ENDPOINT_DEVICE, ENDPOINT_ID, ENDPOINT_BAUD, ENDPOINT_PARITY, ENDPOINT_DATA_BITS, ENDPOINT_STOP_BITS),
		_lightingRelay(_modbusEndpoint, LIGHTING_RELAY_STATE_BIT_ADDR, LIGHTING_RELAY_FEEDBACK_BIT_ADDR),
		_lightingSensor(_modbusEndpoint, LIGHTING_SENSOR_REGISTER_ADDR),
		_timer(this, isl::Timeout(TIMER_CLOCK_TIMEOUT_SECONDS, TIMER_CLOCK_TIMEOUT_NANOSECONDS)),
		_lightningController(_timer, _lightingRelay/*, _lightingSensor*/, isl::Timeout(LIGHTING_CONTROLLER_CLOCK_TIMEOUT_SECONDS, LIGHTING_CONTROLLER_CLOCK_TIMEOUT_NANOSECONDS))
	{
		_modbusEndpoint.open();
		//_networkService.addListener(isl::TcpAddrInfo(isl::TcpAddrInfo::IpV4, isl::TcpAddrInfo::WildcardAddress, 8081));
		//_sensorsService.addListener(isl::TcpAddrInfo(isl::TcpAddrInfo::IpV4, isl::TcpAddrInfo::WildcardAddress, 8082));
	}
private:
	GreenHouseServer();
	GreenHouseServer(const GreenHouseServer&);

	virtual void beforeStart()
	{
		isl::debugLog().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Starting greenhouse server"));
	}
	virtual void afterStart()
	{
		isl::debugLog().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Greenhouse server has been started"));
	}
	virtual void beforeStop()
	{
		isl::debugLog().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Stopping greenhouse server"));
	}
	virtual void afterStop()
	{
		isl::debugLog().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Greenhouse server has been stopped"));
	}

	isl::ModbusEndpoint _modbusEndpoint;
	isl::Relay _lightingRelay;
	isl::Sensor _lightingSensor;
	isl::Timer _timer;
	LightingController _lightningController;
};


#endif
