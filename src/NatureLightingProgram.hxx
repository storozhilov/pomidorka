#ifndef NATURE_LIGHTING_PROGRAM__HXX
#define NATURE_LIGHTING_PROGRAM__HXX 1

#include "AbstractLightingProgram.hxx"

class NatureLightingProgram : public AbstractLightingProgram
{
public:
	//! Creates abstract nature SCADA lighting program
	/*!
	 * \param scadaTimer SCADA timer to register a program in
	 */
	NatureLightingProgram(isl::ScadaTimer& scadaTimer, isl::ModbusEndpoint& modbusEndpoint, int statusRegisterAddr, int intensityRegisterAddr,
			int lightSensorRegisterAddr, int supplyVoltageRegisterAddr, int supplyCurrentRegisterAddr) :
		AbstractLightingProgram(scadaTimer, modbusEndpoint, statusRegisterAddr, intensityRegisterAddr,
				lightSensorRegisterAddr, supplyVoltageRegisterAddr, supplyCurrentRegisterAddr)
	{}
protected:
	//! Returns current light intensity in per-cents: result should be in [0.0 .. 100.0]
	virtual double lightIntensity()
	{
		isl::Timestamp now = isl::Timestamp::now();
		int second = now.second() % 60;
		if (second > 30) {
			second = 60 - second;
		}
		return static_cast<double>(second) / 30.0 * 100.0;
		//return static_cast<double>(now.second() );
		//return 100.0;
	}
};

#endif
