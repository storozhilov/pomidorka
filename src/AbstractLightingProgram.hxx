#ifndef ABSTRACT_LIGHTING_PROGRAM__HXX
#define ABSTRACT_LIGHTING_PROGRAM__HXX 1

#include <isl/ScadaProgram.hxx>
#include <isl/ModbusEndpoint.hxx>
#include <isl/ExceptionLogMessage.hxx>
#include <isl/Timestamp.hxx>

//! Abstract lighting program
/*!
 * TODO: Control (non-frequest clock rate) and Correction (frequent clock rate) tasks support
 */
class AbstractLightingProgram : public isl::ScadaProgram
{
public:
        enum Constants {
                DefaultMaxLightSensorRegisterValue = 255,
                DefaultMaxLampSupplyRegisterValue = 255
        };
        //! Lighting SCADA program state
        struct State
        {
                State(int statusRegisterValue, int lightSensorRegisterValue, int lampSupplyLevelRegisterValue, int lampSupplyVoltageRegisterValue,
                                int lampSupplyCurrentRegisterValue, double currentLightIntensity, double desiredLightIntensity,
                                bool isInManualMode, double manualLampSupplyLevel) :
                        statusRegisterValue(statusRegisterValue),
                        lightSensorRegisterValue(lightSensorRegisterValue),
                        lampSupplyLevelRegisterValue(lampSupplyLevelRegisterValue),
                        lampSupplyVoltageRegisterValue(lampSupplyVoltageRegisterValue),
                        lampSupplyCurrentRegisterValue(lampSupplyCurrentRegisterValue),
                        currentLightIntensity(currentLightIntensity),
                        desiredLightIntensity(desiredLightIntensity),
                        isInManualMode(isInManualMode),
                        manualLampSupplyLevel(manualLampSupplyLevel)
                {}
                State() :
                        statusRegisterValue(0),
                        lightSensorRegisterValue(0),
                        lampSupplyLevelRegisterValue(0),
                        lampSupplyVoltageRegisterValue(0),
                        lampSupplyCurrentRegisterValue(0),
                        currentLightIntensity(0.0),
                        desiredLightIntensity(0.0),
                        isInManualMode(false),
                        manualLampSupplyLevel(0.0)
                {}

                int statusRegisterValue;
                int lightSensorRegisterValue;
                int lampSupplyLevelRegisterValue;
                int lampSupplyVoltageRegisterValue;
                int lampSupplyCurrentRegisterValue;
                double currentLightIntensity;
                double desiredLightIntensity;
                bool isInManualMode;
                double manualLampSupplyLevel;
        };
protected:
	//! State request thread message
	class StateRequest : public isl::Subsystem::AbstractThreadMessage
	{
	public:
		StateRequest() :
                        isl::Subsystem::AbstractThreadMessage("Lighting SCADA program state request")
                {}

		virtual isl::Subsystem::AbstractThreadMessage * clone() const
		{
			return new StateRequest(*this);
		}
	};
	//! State response thread message
	class StateResponse : public isl::Subsystem::AbstractThreadMessage
	{
	public:
		StateResponse(const State& state) :
                        isl::Subsystem::AbstractThreadMessage("Lighting SCADA program state request"),
                        _state(state)
                {}

                //inline const State& state() const
                //inline const State& state()
                inline const State& state() const
                {
                        return _state;
                }

		virtual isl::Subsystem::AbstractThreadMessage * clone() const
		{
			return new StateResponse(*this);
		}
        private:
                const State _state;
	};
public:
	//! Creates abstract SCADA lighting program
	/*!
	 * \param timer SCADA timer to register a program in
	 */
	AbstractLightingProgram(isl::ScadaTimer& timer, isl::ModbusEndpoint& modbusEndpoint, int statusRegisterAddr,
                        int lightSensorRegisterAddr, int lampSupplyLevelRegisterAddr,int lampSupplyVoltageRegisterAddr,
                        int lampSupplyCurrentRegisterAddr, int maxLightSensorRegisterValue = DefaultMaxLightSensorRegisterValue,
                        int maxLampSupplyLevelRegisterValue = DefaultMaxLampSupplyRegisterValue) :
		isl::ScadaProgram(timer),
		_modbusEndpoint(modbusEndpoint),
		_statusRegisterAddr(statusRegisterAddr),
		_lightSensorRegisterAddr(lightSensorRegisterAddr),
		_lampSupplyLevelRegisterAddr(lampSupplyLevelRegisterAddr),
		_lampSupplyVoltageRegisterAddr(lampSupplyVoltageRegisterAddr),
		_lampSupplyCurrentRegisterAddr(lampSupplyCurrentRegisterAddr),
                _maxLightSensorRegisterValue(maxLightSensorRegisterValue),
                _maxLampSupplyLevelRegisterValue(maxLampSupplyLevelRegisterValue),
		_mainPeriodicTask(*this),
                _state(),
                _maxRegisterAddress(0)
	{
		//scadaTimer().registerPeriodicTask(_mainPeriodicTask, isl::Timeout(1));
		//scadaTimer().registerPeriodicTask(_mainPeriodicTask, isl::Timeout(0, 100000000));
		scadaTimer().registerPeriodicTask(_mainPeriodicTask, isl::Timeout(0, 200000000));
	}

        //! Requests lighting SCADA program state
        std::auto_ptr<State> requestState()
        {
                std::auto_ptr<isl::Subsystem::AbstractThreadMessage> stateResponseAutoPtr(sendRequest(StateRequest(), isl::Timestamp::limit(scadaTimer().awaitResponseTimeout())));
                if (!stateResponseAutoPtr.get()) {
                        isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "No response to state request from the SCADA program"));
                        return std::auto_ptr<State>();
                }
                if (!stateResponseAutoPtr->instanceOf<const StateResponse>()) {
                        isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Unrecognized response to state request from the SCADA program: \"") <<
                                        stateResponseAutoPtr->name() << '"');
                        return std::auto_ptr<State>();
                }
                return std::auto_ptr<State>(new State(stateResponseAutoPtr->cast<const StateResponse>()->state()));
        }
protected:
        //! Main periodic task (TODO: Make in ControlPeriodicTask)
	class MainPeriodicTask : public isl::Timer::AbstractPeriodicTask
	{
	public:
		MainPeriodicTask(AbstractLightingProgram& program) :
			isl::Timer::AbstractPeriodicTask(),
			_program(program)
		{}

		virtual void execute(isl::Timer& timer, const isl::Timestamp& lastExpiredTimestamp, size_t expiredTimestamps, const isl::Timeout& timeout)
		{
                        _program.updateState();
                        // TODO Manual mode support
                        if (_program._state.currentLightIntensity < _program._state.desiredLightIntensity) {
                                if (_program._state.lampSupplyLevelRegisterValue < _program._maxLampSupplyLevelRegisterValue) {
                                        int lampSupplyLevelIncrement = _program._state.desiredLightIntensity - _program._state.currentLightIntensity;
                                        if (lampSupplyLevelIncrement > 0) {
                                                int newLampSupplyLevelRegisterValue = _program._state.lampSupplyLevelRegisterValue + lampSupplyLevelIncrement;
                                                if (newLampSupplyLevelRegisterValue > _program._maxLampSupplyLevelRegisterValue) {
                                                        newLampSupplyLevelRegisterValue = _program._maxLampSupplyLevelRegisterValue;
                                                }
                                                isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Current light intensity ") << _program._state.currentLightIntensity <<
                                                                " is less than desired one " << _program._state.desiredLightIntensity << " -> increasing lamp supply level from " <<
                                                                _program._state.lampSupplyLevelRegisterValue << " to " << newLampSupplyLevelRegisterValue);
                                                try {
                                                        _program._modbusEndpoint.writeRegister(_program._lampSupplyLevelRegisterAddr, newLampSupplyLevelRegisterValue);
                                                } catch (isl::Exception& e) {
                                                        isl::Log::error().log(isl::ExceptionLogMessage(SOURCE_LOCATION_ARGS, e, "Error sending write register command"));
                                                }
                                        } else {
                                                isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "System is considered to be stabilized -> nothing to do. Current light intensity: ") <<
                                                                _program._state.currentLightIntensity << ", desired light intensity: " << _program._state.desiredLightIntensity <<
                                                                ", lamp supply level increment: " << lampSupplyLevelIncrement);
                                        }
                                } else {
                                        isl::Log::warning().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Could not increase lamp supply level: upper bound ") <<
                                                        _program._maxLampSupplyLevelRegisterValue << " has been already reached");
                                }
                        } else if (_program._state.currentLightIntensity > _program._state.desiredLightIntensity) {
                                if (_program._state.lampSupplyLevelRegisterValue > 0) {
                                        int lampSupplyLevelDecrement = _program._state.currentLightIntensity - _program._state.desiredLightIntensity;
                                        if (lampSupplyLevelDecrement > 0) {
                                                int newLampSupplyLevelRegisterValue = _program._state.lampSupplyLevelRegisterValue - lampSupplyLevelDecrement;
                                                if (newLampSupplyLevelRegisterValue < 0) {
                                                        newLampSupplyLevelRegisterValue = 0;
                                                }
                                                isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Current light intensity ") << _program._state.currentLightIntensity <<
                                                        " is greater than desired one " << _program._state.desiredLightIntensity << " -> increasing lamp supply level from " <<
                                                        _program._state.lampSupplyLevelRegisterValue << " to " << newLampSupplyLevelRegisterValue);
                                                try {
                                                        _program._modbusEndpoint.writeRegister(_program._lampSupplyLevelRegisterAddr, newLampSupplyLevelRegisterValue);
                                                } catch (isl::Exception& e) {
                                                        isl::Log::error().log(isl::ExceptionLogMessage(SOURCE_LOCATION_ARGS, e, "Error sending write register command"));
                                                }
                                        } else {
                                                isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "System is considered to be stabilized -> nothing to do. Current light intensity: ") <<
                                                                _program._state.currentLightIntensity << ", desired light intensity: " << _program._state.desiredLightIntensity <<
                                                                ", lamp supply level decrement: " << lampSupplyLevelDecrement);
                                        }
                                } else {
                                        isl::Log::warning().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Could not decrease lamp supply level: lower bound 0 has been already reached"));
                                }
                        } else {
                                isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Current light intensity ") << _program._state.currentLightIntensity <<
                                                " is equal to the desired one " << _program._state.desiredLightIntensity << " -> nothing to do");
                                // Nothing to do
                        }
		}
	private:
		AbstractLightingProgram& _program;
	};

        int maxRegisterAddress() const
        {
                if (_maxRegisterAddress <= 0) {
                        if (_statusRegisterAddr > _maxRegisterAddress) {
                                _maxRegisterAddress = _statusRegisterAddr;
                        }
                        if (_lampSupplyLevelRegisterAddr > _maxRegisterAddress) {
                                _maxRegisterAddress = _lampSupplyLevelRegisterAddr;
                        }
                        if (_lightSensorRegisterAddr > _maxRegisterAddress) {
                                _maxRegisterAddress = _lightSensorRegisterAddr;
                        }
                        if (_lampSupplyVoltageRegisterAddr > _maxRegisterAddress) {
                                _maxRegisterAddress = _lampSupplyVoltageRegisterAddr;
                        }
                        if (_lampSupplyCurrentRegisterAddr > _maxRegisterAddress) {
                                _maxRegisterAddress = _lampSupplyCurrentRegisterAddr;
                        }
                }
                return _maxRegisterAddress;
        }

        virtual std::auto_ptr<isl::Subsystem::AbstractThreadMessage> onRequest(const isl::Subsystem::AbstractThreadMessage& request, bool responseRequired)
        {
                if (request.instanceOf<const StateRequest>()) {
                        isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Status request has been received by the lighting SCADA program"));
                        return std::auto_ptr<isl::Subsystem::AbstractThreadMessage>(new StateResponse(_state));
                } else {
                        return ScadaProgram::onRequest(request, responseRequired);
                }
        }

	//! Returns desired light intensity in per-cents: result should be in [0.0 .. 100.0]
        /*!
         * \note To be called from the timer's thread only
         */
	virtual double desiredLightIntensity() = 0;
private:
        void updateState()
        {
                try {
                        std::vector<uint16_t> registers = _modbusEndpoint.readInputRegisters(0, maxRegisterAddress() - 1);
                        _state = State(
                                registers[_statusRegisterAddr],
                                registers[_lightSensorRegisterAddr],
                                registers[_lampSupplyLevelRegisterAddr],
                                registers[_lampSupplyVoltageRegisterAddr],
                                registers[_lampSupplyCurrentRegisterAddr],
                                static_cast<double>(registers[_lightSensorRegisterAddr]) * 100.0 / static_cast<double>(_maxLightSensorRegisterValue),
                                desiredLightIntensity(),
                                _state.isInManualMode,
                                _state.manualLampSupplyLevel
                        );
                } catch (isl::Exception& e) {
                        isl::Log::error().log(isl::ExceptionLogMessage(SOURCE_LOCATION_ARGS, e, "Error sending write register command"));
                }
        }

	isl::ModbusEndpoint& _modbusEndpoint;
	int _statusRegisterAddr;
	int _lightSensorRegisterAddr;
	int _lampSupplyLevelRegisterAddr;
	int _lampSupplyVoltageRegisterAddr;
	int _lampSupplyCurrentRegisterAddr;
        const int _maxLightSensorRegisterValue;
        const int _maxLampSupplyLevelRegisterValue;
	MainPeriodicTask _mainPeriodicTask;
        State _state;
        mutable int _maxRegisterAddress;
};

#endif
