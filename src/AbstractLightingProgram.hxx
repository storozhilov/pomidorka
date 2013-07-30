#ifndef ABSTRACT_LIGHTING_PROGRAM__HXX
#define ABSTRACT_LIGHTING_PROGRAM__HXX 1

#include <isl/ScadaProgram.hxx>
#include <isl/ModbusEndpoint.hxx>

class AbstractLightingProgram : public isl::ScadaProgram
{
public:
	//! Creates abstract SCADA lighting program
	/*!
	 * \param timer SCADA timer to register a program in
	 */
	AbstractLightingProgram(isl::ScadaTimer& timer, isl::ModbusEndpoint& modbusEndpoint, int statusRegisterAddr, int intensityRegisterAddr,
			int lightSensorRegisterAddr, int supplyVoltageRegisterAddr, int supplyCurrentRegisterAddr) :
		isl::ScadaProgram(timer),
		_modbusEndpoint(modbusEndpoint),
		_statusRegisterAddr(statusRegisterAddr),
		_intensityRegisterAddr(intensityRegisterAddr),
		_lightSensorRegisterAddr(lightSensorRegisterAddr),
		_supplyVoltageRegisterAddr(supplyVoltageRegisterAddr),
		_supplyCurrentRegisterAddr(supplyCurrentRegisterAddr),
		_mainPeriodicTask(*this)
	{
		scadaTimer().registerPeriodicTask(_mainPeriodicTask, isl::Timeout(1));
	}
protected:
	class MainPeriodicTask : public isl::Timer::AbstractPeriodicTask
	{
	public:
		MainPeriodicTask(AbstractLightingProgram& program) :
			isl::Timer::AbstractPeriodicTask(),
			_program(program)
		{}

		virtual void execute(isl::Timer& timer, const isl::Timestamp& lastExpiredTimestamp, size_t expiredTimestamps, const isl::Timeout& timeout)
		{
			double intensity = _program.lightIntensity();
			isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Main task execution method has been called. Light intensity is ") << intensity << '.');
			_program._modbusEndpoint.writeRegister(_program._intensityRegisterAddr, static_cast<uint16_t>(intensity * 255 / 100));
			isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "First MODBUS command has been sent"));
			_program._modbusEndpoint.writeRegister(_program._intensityRegisterAddr, static_cast<uint16_t>(intensity * 255 / 100));
			isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Second MODBUS command has been sent"));
			//_program._modbusEndpoint.writeRegister(1, 0);
			//me.writeRegister(registerAddr, registerValue);
		}
	private:
		AbstractLightingProgram& _program;
	};

	//! Returns current light intensity in per-cents: result should be in [0.0 .. 100.0]
	virtual double lightIntensity() = 0;
private:
	isl::ModbusEndpoint& _modbusEndpoint;
	int _statusRegisterAddr;
	int _intensityRegisterAddr;
	int _lightSensorRegisterAddr;
	int _supplyVoltageRegisterAddr;
	int _supplyCurrentRegisterAddr;
	MainPeriodicTask _mainPeriodicTask;
};

#endif
