#ifndef LIGHTING_SUBSYSTEM__HXX
#define LIGHTING_SUBSYSTEM__HXX

#include <isl/Subsystem.hxx>
#include <isl/ModbusEndpoint.hxx>
#include <isl/FsmThread.hxx>
#include <cmath>

#define MONITORING_STATE_TIMEOUT 1.0

class LightingSubsystem : public isl::Subsystem
{
public:
        enum Constants {
                DefaultMaxLightSensorRegisterValue = 255,
                DefaultMaxLampSupplyRegisterValue = 255
        };

        struct Status
        {
                int statusRegisterValue;
                int lightSensorRegisterValue;
                double desiredLightIntensity;
                double currentLightIntensity;
                int lampSupplyLevelRegisterValue;
                int lampSupplyVoltageRegisterValue;
                int lampSupplyCurrentRegisterValue;
                bool isManualDesiredLightIntensity;
                double manualDesiredLightIntensity;
                bool isManualLampSupplyLevel;
                double manualLampSupplyLevel;
                
                Status() :
                        statusRegisterValue(0),
                        lightSensorRegisterValue(0),
                        desiredLightIntensity(0.0),
                        currentLightIntensity(0.0),
                        lampSupplyLevelRegisterValue(0),
                        lampSupplyVoltageRegisterValue(0),
                        lampSupplyCurrentRegisterValue(0),
                        isManualDesiredLightIntensity(false),
                        manualDesiredLightIntensity(0.0),
                        isManualLampSupplyLevel(false),
                        manualLampSupplyLevel(0.0)
                {}
        };

        LightingSubsystem(isl::Subsystem * owner, isl::ModbusEndpoint& modbusEndpoint, int statusRegisterAddr,
                        int lightSensorRegisterAddr, int lampSupplyLevelRegisterAddr,int lampSupplyVoltageRegisterAddr,
                        int lampSupplyCurrentRegisterAddr, int maxLightSensorRegisterValue = DefaultMaxLightSensorRegisterValue,
                        int maxLampSupplyLevelRegisterValue = DefaultMaxLampSupplyRegisterValue) :
                isl::Subsystem(owner),
                _modbusEndpoint(modbusEndpoint),
                _monitoringState(*this),
                _correctionState(*this),
                _lightingThread(*this, _monitoringState),
		_statusRegisterAddr(statusRegisterAddr),
		_lightSensorRegisterAddr(lightSensorRegisterAddr),
		_lampSupplyLevelRegisterAddr(lampSupplyLevelRegisterAddr),
		_lampSupplyVoltageRegisterAddr(lampSupplyVoltageRegisterAddr),
		_lampSupplyCurrentRegisterAddr(lampSupplyCurrentRegisterAddr),
                _maxLightSensorRegisterValue(maxLightSensorRegisterValue),
                _maxLampSupplyLevelRegisterValue(maxLampSupplyLevelRegisterValue),
                _status(),
                _maxRegisterAddress(0)
        {}

        std::auto_ptr<Status> requestStatus()
        {
                std::auto_ptr<isl::Subsystem::AbstractThreadMessage> responseAutoPtr(_lightingThread.sendRequest(StatusRequest(), isl::Timestamp::limit(awaitResponseTimeout())));
                if (!responseAutoPtr.get()) {
                        isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "No response to state request from the lighting subsystem"));
                        return std::auto_ptr<Status>();
                }
                if (!responseAutoPtr->instanceOf<const StatusResponse>()) {
                        isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Unrecognized response to state request from the lighting subsystem: \"") <<
                                        responseAutoPtr->name() << '"');
                        return std::auto_ptr<Status>();
                }
                return std::auto_ptr<Status>(new Status(responseAutoPtr->cast<const StatusResponse>()->status()));
        }

        bool setManualLampSupplyLevel(int level)
        {
                std::auto_ptr<isl::Subsystem::AbstractThreadMessage> responseAutoPtr(_lightingThread.sendRequest(SetManualLampSupplyLevelRequest(level),
                                        isl::Timestamp::limit(awaitResponseTimeout())));
                if (!responseAutoPtr.get()) {
                        isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "No response to set manual lamp supply level request from the lighting subsystem"));
                        return false;
                }
                if (!responseAutoPtr->instanceOf<const OkResponse>()) {
                        isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Unrecognized response to set manual lamp supply level request from the lighting subsystem: \"") <<
                                        responseAutoPtr->name() << '"');
                        return false;
                }
                return true;
        }

        bool resetManualLampSupplyLevel()
        {
                std::auto_ptr<isl::Subsystem::AbstractThreadMessage> responseAutoPtr(_lightingThread.sendRequest(ResetManualLampSupplyLevelRequest(),
                                        isl::Timestamp::limit(awaitResponseTimeout())));
                if (!responseAutoPtr.get()) {
                        isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "No response to reset manual lamp supply level request from the lighting subsystem"));
                        return false;
                }
                if (!responseAutoPtr->instanceOf<const OkResponse>()) {
                        isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Unrecognized response to reset manual lamp supply level request from the lighting subsystem: \"") <<
                                        responseAutoPtr->name() << '"');
                        return false;
                }
                return true;
        }

protected:
	//! Returns desired light intensity in per-cents: result should be in [0.0 .. 100.0]
        /*!
         * \note To be called from the subsystem's thread only
         */
	virtual double desiredLightIntensity()
        {
                return 50.0;
        }
private:
        class AbstractState : public isl::FsmThread::AbstractState
        {
        public:
                AbstractState(LightingSubsystem& subsystem, const std::string& name) :
                        isl::FsmThread::AbstractState(name),
                        _subsystem(subsystem)
                {}
        protected:
                inline LightingSubsystem& subsystem() const
                {
                        return _subsystem;
                }
        private:
                LightingSubsystem& _subsystem;
        };

        class MonitoringState : public AbstractState
        {
        public:
                MonitoringState(LightingSubsystem& subsystem) :
                        AbstractState(subsystem, "Lighting subsystem monitoring state")
                {}
        private:
                virtual isl::FsmThread::NextStep makeStep(isl::FsmThread& fsmThread)
                {
                        // Updating status
                        std::vector<uint16_t> registers;
                        try {
                                registers = subsystem()._modbusEndpoint.readInputRegisters(0, subsystem().maxRegisterAddress() - 1);
                        } catch (isl::Exception& e) {
                                isl::Log::error().log(isl::ExceptionLogMessage(SOURCE_LOCATION_ARGS, e, "Error sending read input registers command"));
                                // TODO: Set to error state?
                                return isl::FsmThread::NextStep(*this, isl::Timeout(MONITORING_STATE_TIMEOUT));
                        }
                        subsystem()._status.statusRegisterValue = registers[subsystem()._statusRegisterAddr];
                        subsystem()._status.lightSensorRegisterValue = registers[subsystem()._lightSensorRegisterAddr];
                        subsystem()._status.desiredLightIntensity = subsystem().desiredLightIntensity();
                        subsystem()._status.currentLightIntensity = static_cast<double>(subsystem()._status.lightSensorRegisterValue) * 100.0 /
                                static_cast<double>(subsystem()._maxLightSensorRegisterValue);
                        subsystem()._status.lampSupplyLevelRegisterValue = registers[subsystem()._lampSupplyLevelRegisterAddr];
                        subsystem()._status.lampSupplyVoltageRegisterValue = registers[subsystem()._lampSupplyVoltageRegisterAddr];
                        subsystem()._status.lampSupplyCurrentRegisterValue = registers[subsystem()._lampSupplyCurrentRegisterAddr];
                        // Check out for manual lamp supply level state
                        if (subsystem()._status.isManualLampSupplyLevel) {
                                if (subsystem()._status.manualLampSupplyLevel != subsystem()._status.lampSupplyLevelRegisterValue) {
                                        isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Manual lamp supply level (") <<
                                                        subsystem()._status.manualLampSupplyLevel << ") is differs from actual one (" <<
                                                        subsystem()._status.lampSupplyLevelRegisterValue << ") -> falling to correction state");
                                        return isl::FsmThread::NextStep(subsystem()._correctionState, isl::Timeout());
                                } else {
                                        return isl::FsmThread::NextStep(*this, isl::Timeout(MONITORING_STATE_TIMEOUT));
                                }
                        }
                        // Doing the correction if needed
                        if (fabs(subsystem()._status.currentLightIntensity - subsystem()._status.desiredLightIntensity) <= 1.0) {
                                // No correction needed
                                isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Current light intensity ") <<
                                                subsystem()._status.currentLightIntensity << ") is closer enough to the desired one (" <<
                                                subsystem()._status.desiredLightIntensity << ") -> nothing to do");
                                return isl::FsmThread::NextStep(*this, isl::Timeout(MONITORING_STATE_TIMEOUT));
                        }
                        // Correction needed
                        isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Correction needed from current light intensity (") <<
                                        subsystem()._status.currentLightIntensity << ") to the desired one (" << subsystem()._status.desiredLightIntensity << ')');
                        // Checking limits
                        if (subsystem()._status.currentLightIntensity < subsystem()._status.desiredLightIntensity &&
                                        subsystem()._status.lampSupplyLevelRegisterValue >= subsystem()._maxLampSupplyLevelRegisterValue) {
                                isl::Log::warning().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Lamp supply level is already maximum (") <<
                                                subsystem()._maxLampSupplyLevelRegisterValue << ") -> nothing to do");
                                return isl::FsmThread::NextStep(*this, isl::Timeout(MONITORING_STATE_TIMEOUT));
                        }
                        if (subsystem()._status.currentLightIntensity > subsystem()._status.desiredLightIntensity && subsystem()._status.lampSupplyLevelRegisterValue <= 0) {
                                isl::Log::warning().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Lamp supply level is already minimum (0) -> nothing to do"));
                                return isl::FsmThread::NextStep(*this, isl::Timeout(MONITORING_STATE_TIMEOUT));
                        }
                        // Falling to the correction state
                        return isl::FsmThread::NextStep(subsystem()._correctionState, isl::Timeout());
                }
        };

        class CorrectionState : public AbstractState
        {
        public:
                CorrectionState(LightingSubsystem& subsystem) :
                        AbstractState(subsystem, "Lighting subsystem correction state")
                {}
        private:
                //! Makes a step and returns a pointer to the next state
                virtual isl::FsmThread::NextStep makeStep(isl::FsmThread& fsmThread)
                {
                        int newLampSupplyLevelRegisterValue = 0;
                        if (subsystem()._status.isManualLampSupplyLevel) {
                                newLampSupplyLevelRegisterValue = subsystem()._status.manualLampSupplyLevel;
                        } else {
                                int lampSupplyLevelCorrection = (subsystem()._status.desiredLightIntensity - subsystem()._status.currentLightIntensity) * 100 /
                                        subsystem()._maxLampSupplyLevelRegisterValue;
                                newLampSupplyLevelRegisterValue = subsystem()._status.lampSupplyLevelRegisterValue + lampSupplyLevelCorrection;
                                if (newLampSupplyLevelRegisterValue < 0) {
                                        newLampSupplyLevelRegisterValue = 0;
                                }
                                if (newLampSupplyLevelRegisterValue > subsystem()._maxLampSupplyLevelRegisterValue) {
                                        newLampSupplyLevelRegisterValue = subsystem()._maxLampSupplyLevelRegisterValue;
                                }
                        }
                        try {
                                subsystem()._modbusEndpoint.writeRegister(subsystem()._lampSupplyLevelRegisterAddr, newLampSupplyLevelRegisterValue);
                                isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Lamp supply level has been corrected from ") <<
                                                subsystem()._status.lampSupplyLevelRegisterValue << " to " << newLampSupplyLevelRegisterValue <<
                                                " -> switching to monitoring state");
                        } catch (isl::Exception& e) {
                                isl::Log::error().log(isl::ExceptionLogMessage(SOURCE_LOCATION_ARGS, e, "Error sending write register command"));
                                // TODO: Set to error state?
                        }
                        // Falling to monitoring state
                        return isl::FsmThread::NextStep(subsystem()._monitoringState, isl::Timeout());
                }
        };

	//! Status request thread message
	class StatusRequest : public AbstractThreadMessage
	{
	public:
		StatusRequest() :
                        isl::Subsystem::AbstractThreadMessage("Lighting subsystem status request")
                {}

		virtual isl::Subsystem::AbstractThreadMessage * clone() const
		{
			return new StatusRequest(*this);
		}
	};

	//! Status response thread message
	class StatusResponse : public isl::Subsystem::AbstractThreadMessage
	{
	public:
		StatusResponse(const Status& status) :
                        isl::Subsystem::AbstractThreadMessage("Lighting subsystem status response"),
                        _status(status)
                {}

                inline const Status& status() const
                {
                        return _status;
                }

		virtual isl::Subsystem::AbstractThreadMessage * clone() const
		{
			return new StatusResponse(*this);
		}
        private:
                const Status _status;
	};

	//! Set manual lamp supply level request
	class SetManualLampSupplyLevelRequest : public isl::Subsystem::AbstractThreadMessage
	{
	public:
		SetManualLampSupplyLevelRequest(int level) :
                        isl::Subsystem::AbstractThreadMessage("Lighting subsystem set manual lamp supply level request"),
                        _level(level)
                {}

                inline int level() const
                {
                        return _level;
                }

		virtual isl::Subsystem::AbstractThreadMessage * clone() const
		{
			return new SetManualLampSupplyLevelRequest(*this);
		}
        private:
                const int _level;
	};

	//! Reset manual lamp supply level request
	class ResetManualLampSupplyLevelRequest : public isl::Subsystem::AbstractThreadMessage
	{
	public:
		ResetManualLampSupplyLevelRequest() :
                        isl::Subsystem::AbstractThreadMessage("Lighting subsystem reset manual lamp supply level request")
                {}

		virtual isl::Subsystem::AbstractThreadMessage * clone() const
		{
			return new ResetManualLampSupplyLevelRequest(*this);
		}
	};

        class LightingThread : public isl::FsmThread
        {
        public:
                LightingThread(LightingSubsystem& lightingSubsystem, AbstractState& initialState) :
                        isl::FsmThread(lightingSubsystem, initialState),
                        _lightingSubsystem(lightingSubsystem)
                {}
        protected:
                virtual std::auto_ptr<isl::Subsystem::AbstractThreadMessage> onRequest(const isl::Subsystem::AbstractThreadMessage& request, bool responseRequired, bool& stopRequestsProcessing)
                {
                        if (request.instanceOf<const StatusRequest>()) {
                                isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Status request has been received by the lighting subsystem"));
                                return std::auto_ptr<isl::Subsystem::AbstractThreadMessage>(new StatusResponse(_lightingSubsystem._status));
                        } else if (request.instanceOf<const SetManualLampSupplyLevelRequest>()) {
                                isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Set manual lamp suply level request has been received by the lighting subsystem"));
                                _lightingSubsystem._status.isManualLampSupplyLevel = true;
                                _lightingSubsystem._status.manualLampSupplyLevel = request.cast<const SetManualLampSupplyLevelRequest>()->level();
                                if (_lightingSubsystem._status.manualLampSupplyLevel < 0) {
                                        _lightingSubsystem._status.manualLampSupplyLevel = 0;
                                }
                                if (_lightingSubsystem._status.manualLampSupplyLevel > _lightingSubsystem._maxLampSupplyLevelRegisterValue) {
                                        _lightingSubsystem._status.manualLampSupplyLevel = _lightingSubsystem._maxLampSupplyLevelRegisterValue;
                                }
                                if (_lightingSubsystem._status.lampSupplyLevelRegisterValue != _lightingSubsystem._status.manualLampSupplyLevel) {
                                        appointNextState(NextStep(_lightingSubsystem._correctionState, isl::Timeout()));
                                        stopRequestsProcessing = true;
                                }
                                return std::auto_ptr<isl::Subsystem::AbstractThreadMessage>(new OkResponse());
                        } else if (request.instanceOf<const ResetManualLampSupplyLevelRequest>()) {
                                isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Reset manual lamp suply level request has been received by the lighting subsystem"));
                                _lightingSubsystem._status.isManualLampSupplyLevel = false;
                                appointNextState(NextStep(_lightingSubsystem._correctionState, isl::Timeout()));
                                stopRequestsProcessing = true;
                                return std::auto_ptr<isl::Subsystem::AbstractThreadMessage>(new OkResponse());
                        } else {
                                return isl::FsmThread::onRequest(request, responseRequired, stopRequestsProcessing);
                        }
                }
        private:
                LightingSubsystem& _lightingSubsystem;
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

        isl::ModbusEndpoint& _modbusEndpoint;
        MonitoringState _monitoringState;
        CorrectionState _correctionState;

        LightingThread _lightingThread;
	int _statusRegisterAddr;
	int _lightSensorRegisterAddr;
	int _lampSupplyLevelRegisterAddr;
	int _lampSupplyVoltageRegisterAddr;
	int _lampSupplyCurrentRegisterAddr;
        const int _maxLightSensorRegisterValue;
        const int _maxLampSupplyLevelRegisterValue;
        Status _status;
        mutable int _maxRegisterAddress;
};

#endif
