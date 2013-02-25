#ifndef LIGHTING
#define LIGHTING

#include <isl/Timer.hxx>
#include <isl/Relay.hxx>
#include <isl/Sensor.hxx>
#include <isl/Exception.hxx>
#include <isl/ExceptionLogMessage.hxx>
//#include <isl/DateTime.hxx>

#include <iostream>

class LightingController
{
public:
	LightingController(isl::Timer& timer, isl::Relay& lightRelay, const isl::Timeout& clockTimeout) :
		_timer(timer),
		_lightRelay(lightRelay),
		_clockTimeout(clockTimeout),
		_timerTask(*this),
		_timerTaskId()
	{
		_timerTaskId = _timer.registerPeriodicTask(_timerTask, _clockTimeout);
	}
	~LightingController()
	{
		_timer.removePeriodicTask(_timerTaskId);
	}

	//int addLightPeriod(const isl::DateTime& startingFrom, const isl::DateTime& finishedBefore);
	//void removeLightPeriod(int lightPeriodId);
	//void resetLightPeriods();

	class TimerTask : public isl::Timer::AbstractPeriodicTask
	{
	public:
		TimerTask(LightingController& controller) :
			isl::Timer::AbstractPeriodicTask(),
			_controller(controller)
		{}
	private:
		TimerTask();

		virtual void execute(isl::Timer& timer, const isl::Timestamp& lastExpiredTimestamp, size_t expiredTimestamps, const isl::Timeout& timeout)
		{
			isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Lighting controller task has been executed"));
			//isl::DateTime now = isl::DateTime::now().time();	// TODO Error here!!!
			try {
				bool currentRelayState = _controller._lightRelay.state();
				//if (_controller.shouldBeLight(now)) {
				if (_controller.shouldBeLight(isl::DateTime::now())) {
					// Light should be on
					isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light sould be ON"));
					// TODO Check out light sensor value!
					if (!currentRelayState) {
						// Light is off -> turn it on
						isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light is OFF -> turn it ON"));
						_controller._lightRelay.setState(true);
					}
					// Checkout new light relay state
					if (!_controller._lightRelay.state()) {
						isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light relay state is off after turning the light on"));
					}
					// Checkout relay feedback channel state if available
					if (_controller._lightRelay.hasFeedback() && !_controller._lightRelay.feedbackState()) {
						isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light is on but feedback channel is off"));
					}
				} else {
					// Light should be off
					isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light sould be OFF"));
					if (currentRelayState) {
						// Light is on -> turn it off
						isl::Log::debug().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light is ON -> turn it OFF"));
						_controller._lightRelay.setState(false);
					}
					// Checkout new light relay state
					if (_controller._lightRelay.state()) {
						isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light relay state is on after turning the light off"));
					}
					// Checkout relay feedback channel state if available
					if (_controller._lightRelay.hasFeedback() && _controller._lightRelay.feedbackState()) {
						isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light is off but feedback channel is on"));
					}
				}
			//} catch (isl::Exception& e) {
			} catch (std::exception& e) {
				isl::Log::error().log(isl::ExceptionLogMessage(SOURCE_LOCATION_ARGS, e, "Lighting controller timer task execution error"));
			} catch (...) {
				isl::Log::error().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Lighting controller timer task execution unknown error"));
			}
		}

		LightingController& _controller;
	};

private:
	LightingController();
	LightingController(const LightingController&);					// No copy

	LightingController& operator=(const LightingController&);			// No copy

	bool shouldBeLight(const isl::DateTime& dt)
	{
		// TODO Check periods

		std::cout << "dt.toString() = " << dt.toString() << ", dt.minute() = " << dt.minute() << ", dt.minute() % 2 = " << dt.minute() % 2 << std::endl;

		return dt.minute() % 2 == 0;						// Switch light every minute at the moment
	}

	isl::Timer& _timer;
	isl::Relay& _lightRelay;
	//isl::Sensor _lightSensor;	// TODO
	isl::Timeout _clockTimeout;
	TimerTask _timerTask;
	int _timerTaskId;
};

#endif
