#ifndef LIGHTING
#define LIGHTING

#include <isl/Timer.hxx>
#include <isl/Relay.hxx>
#include <isl/Sensor.hxx>
#include <isl/Exception.hxx>
#include <isl/ExceptionLogMessage.hxx>
//#include <isl/DateTime.hxx>

class LightingController
{
public:
	LightingController(isl::Timer& timer, isl::Relay& lampRelay, const isl::Timeout& clockTimeout) :
		_timer(timer),
		_lampRelay(lampRelay),
		_clockTimeout(clockTimeout),
		_timerTask(*this)
	{
		_timer.registerPeriodicTask(_timerTask, _clockTimeout);
	}

	//int addLightPeriod(const isl::DateTime& startingFrom, const isl::DateTime& finishedBefore);
	//void removeLightPeriod(int lightPeriodId);
	//void resetLightPeriods();

	class TimerTask : public isl::Timer::AbstractTask
	{
	public:
		TimerTask(LightingController& controller) :
			isl::Timer::AbstractTask(),
			_controller(controller)
		{}
	private:
		TimerTask();

		virtual void execute(isl::Timer& timer, const struct timespec& lastExpiredTimestamp, size_t expiredTimestamps, const isl::Timeout& timeout)
		{
			isl::debugLog().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Lighting controller task has been executed"));
			bool currentRelayState = _controller._lampRelay.state();
			isl::DateTime nowTime = isl::DateTime::now().time();
			try {
				// TODO Check periods and light sensor value!!!
				if (nowTime.minute() % 2 == 0) {	// Switch light every minute at the moment
					// Light is on
					if (!currentRelayState) {
						// Light is off -> turn it on
						_controller._lampRelay.setState(true);
					}
					// Checkout relay feedback channel state
					if (_controller._lampRelay.hasFeedback() && !_controller._lampRelay.feedbackState()) {
						isl::errorLog().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light is on but feedback channel is off"));
					}
				} else {
					// Light is off
					if (currentRelayState) {
						// Light is on -> turn it off
						_controller._lampRelay.setState(false);
					}
					// Checkout relay feedback channel state
					if (_controller._lampRelay.hasFeedback() && _controller._lampRelay.feedbackState()) {
						isl::errorLog().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light is off but feedback channel is on"));
					}
				}
			//} catch (isl::Exception& e) {
			} catch (std::exception& e) {
				isl::errorLog().log(isl::ExceptionLogMessage(SOURCE_LOCATION_ARGS, e, "Lighting controller timer task execution error"));
			} catch (...) {
				isl::errorLog().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Lighting controller timer task execution unknown error"));
			}
		}

		LightingController& _controller;
	};

private:
	LightingController();
	LightingController(const LightingController&);					// No copy

	LightingController& operator=(const LightingController&);			// No copy

	isl::Timer& _timer;
	isl::Relay& _lampRelay;
	//isl::Sensor _lightSensor;	// TODO
	isl::Timeout _clockTimeout;
	TimerTask _timerTask;
};

#endif
