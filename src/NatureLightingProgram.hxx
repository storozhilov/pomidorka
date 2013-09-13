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
	NatureLightingProgram(isl::ScadaTimer& scadaTimer, isl::ModbusEndpoint& modbusEndpoint, int statusRegisterAddr, int lightSensorRegisterAddr,
                        int lampSupplyLevelRegisterAddr, int supplyVoltageRegisterAddr, int supplyCurrentRegisterAddr) :
		AbstractLightingProgram(scadaTimer, modbusEndpoint, statusRegisterAddr, lightSensorRegisterAddr,
                                lampSupplyLevelRegisterAddr, supplyVoltageRegisterAddr, supplyCurrentRegisterAddr)
	{}
protected:
	//! Returns current desired light intensity in per-cents (0.0-100.0)
        /*!
         * TODO: Implementation should return intensity from particular place on Earth
         */
	virtual double desiredLightIntensity()
	{
		/*isl::Timestamp now = isl::Timestamp::now();
		
		int level = now.second() % 10 * 10 + now.nanoSecond() / 100000000L;
		if (level > 50) {
			level = 100 - level;
		}
		return static_cast<double>(level) * 2.0;*/

                return 50.0;
	}
};

#endif
