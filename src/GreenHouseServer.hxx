#ifndef GREEN_HOUSE_SERVER__HXX
#define GREEN_HOUSE_SERVER__HXX 1

#include <isl/Server.hxx>
#include <isl/LogMessage.hxx>
#include <isl/ModbusEndpoint.hxx>
#include "LightingSubsystem.hxx"
#include "HttpService.hxx"

#define ENDPOINT_DEVICE "/dev/ttyUSB0"
#define ENDPOINT_ID 1
#define ENDPOINT_BAUD isl::ModbusEndpoint::Baud_38400
#define ENDPOINT_PARITY isl::ModbusEndpoint::EvenParity
#define ENDPOINT_DATA_BITS isl::ModbusEndpoint::EightDataBits
#define ENDPOINT_STOP_BITS isl::ModbusEndpoint::OneStopBit
#define ENDPOINT_IDLE_TOMEOUT 0.02

#define LIGHTING_STATUS_REGISTER_ADDRESS 0
#define LIGHTING_LAMP_SUPPLY_LEVEL_REGISTER_ADDRESS 1
#define LIGHTING_LIGHT_SENSOR_REGISTER_ADDRESS 2
#define LIGHTING_LAMP_SUPPLY_VOLTAGE_REGISTER_ADDRESS 3
#define LIGHTING_LAMP_SUPPLY_CURRENT_REGISTER_ADDRESS 4
#define LIGHTING_SENSOR_REGISTER_ADDR 4

#define HTTP_LISTEN_PORT 8080
#define HTTP_MAX_CLIENTS 10
#define HTTP_DATA_TRANSMISSION_TIMEOUT 10.0
//#define HTTP_HTDOCS_DIR "/var/lib/ghd"
#define HTTP_HTDOCS_DIR "../htdocs"

class GreenHouseServer : public isl::Server
{
public:
	GreenHouseServer(int argc, char * argv[]) :
		isl::Server(argc, argv),
		_modbusEndpoint(ENDPOINT_DEVICE, ENDPOINT_ID, ENDPOINT_BAUD, ENDPOINT_PARITY, ENDPOINT_DATA_BITS, ENDPOINT_STOP_BITS,
                                isl::Timeout(ENDPOINT_IDLE_TOMEOUT)),
                _lightingSubsystem(this, _modbusEndpoint, LIGHTING_STATUS_REGISTER_ADDRESS, LIGHTING_LIGHT_SENSOR_REGISTER_ADDRESS,
                                LIGHTING_LAMP_SUPPLY_LEVEL_REGISTER_ADDRESS, LIGHTING_LAMP_SUPPLY_VOLTAGE_REGISTER_ADDRESS,
                                LIGHTING_LAMP_SUPPLY_CURRENT_REGISTER_ADDRESS),
                _httpService(this, _lightingSubsystem, HTTP_MAX_CLIENTS, HTTP_HTDOCS_DIR, isl::Timeout(HTTP_DATA_TRANSMISSION_TIMEOUT))
	{
		_modbusEndpoint.open();
                _httpService.addListener(isl::TcpAddrInfo(isl::TcpAddrInfo::IpV4, isl::TcpAddrInfo::WildcardAddress, HTTP_LISTEN_PORT));
	}
private:
	GreenHouseServer();
	GreenHouseServer(const GreenHouseServer&);

	virtual void beforeStart()
	{
		isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Starting greenhouse server"));
	}
	virtual void afterStart()
	{
		isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Greenhouse server has been started"));
	}
	virtual void beforeStop()
	{
		isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Stopping greenhouse server"));
	}
	virtual void afterStop()
	{
		isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Greenhouse server has been stopped"));
	}

	isl::ModbusEndpoint _modbusEndpoint;
        LightingSubsystem _lightingSubsystem;
        HttpService _httpService;
};


#endif
