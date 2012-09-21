#ifndef LIGHTING
#define LIGHTING

#include <isl/Timer.hxx>
#include <isl/Relay.hxx>
#include <isl/Sensor.hxx>
//#include <isl/DateTime.hxx>

class LightingController
{
public:
	LightingController(isl::Timer& timer, isl::Relay& relay, const isl::Timeout& clockTimeout) :
		_timer(timer),
		_relay(relay),
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
			bool currentRelayState = _controller._relay.state();
			isl::DateTime nowTime = isl::DateTime::now().time();
			// TODO Check periods and light sensor value!!!
			if (nowTime >= isl::DateTime(0, 0, 0, 10, 0, 0) && nowTime < isl::DateTime(0, 0, 0, 17, 0, 0)) {
				// Light is on
				if (!currentRelayState) {
					// Light is off -> turn it on
					_controller._relay.setState(true);
				}
				// Checkout relay feedback channel state
				if (_controller._relay.hasFeedback() && !_controller._relay.feedbackState()) {
					isl::errorLog().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light is on but feedback channel is off"));
				}
			} else {
				// Light is off
				if (currentRelayState) {
					// Light is on -> turn it off
					_controller._relay.setState(false);
				}
				// Checkout relay feedback channel state
				if (_controller._relay.hasFeedback() && _controller._relay.feedbackState()) {
					isl::errorLog().log(isl::LogMessage(SOURCE_LOCATION_ARGS, "Light is off but feedback channel is on"));
				}
			}
		}

		LightingController& _controller;
	};

private:
	LightingController();
	LightingController(const LightingController&);					// No copy

	LightingController& operator=(const LightingController&);			// No copy

	isl::Timer& _timer;
	isl::Relay& _relay;
	isl::Timeout _clockTimeout;
	TimerTask _timerTask;
};

#endif
