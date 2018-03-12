#include "crc_pwmreader.h"

using namespace C_RC;

//----------------------------------------------------------------------//
template <class T>
PwmReader<T>::PwmReader(Owner* o,
			P_WP::PinNum pin,
			PwmLimits<T> limits)
  : d_owner(o),
    d_map(std::move(limits))
{
  // setup interrupt
  d_interrupt.reset(new P_WP::Interrupt(pin,
					P_WP::Interrupt::Mode::Both,
					[this](){
					  processInterrupt();
					}));

  if (d_interrupt->hasError())
    {
      d_error_timer.singleShotZero([this](){
	  d_owner.call(PwmReader::handleError,
		       Error::InternalInterruptSetup,
		       "PwmReader::PwmReader - Failed to setup the interrupt.");
	});
    }

  // This thread-safe timer is used to call commands in the main thread by using a zero time.
  d_update_timer.setTimeoutCallback([this](){
      ownerPulseDuration();
    });
}
    
//----------------------------------------------------------------------//
template <class T>
void PwmReader<T>::ownerPulseDuration()
{
  std::chrono::microseconds dur(0);
  if (!d_pulse_durations.tryPopFront(dur))
    {
      return;
    }

  if (!d_map.isDurationValid(dur))
    {
      assert(false);
      std::ostringstream stream("PwmReader::ownerPulseDuration - The measured pulse duration of ",
				std::ios_base::app);
      stream << dur.count() << " [us] is invalid because it is out of bounds. Max [us] = "
	     << d_map.getPwmLimits().max_duration_micros << ", Min [us] = "
	     << d_map.getPwmLimits().min_duration_micros << ".";
      ownerError(Error::InvalidDuration, stream.str());
      return;
    }
  d_owner.call(PwmReader::handlePulseDuration, this, d_map.getValue(dur));
}

//----------------------------------------------------------------------//
template <class T>
void PwmReader<T>::processInterrupt()
{
  // MAKE SURE THE PRIVATE MEMBERS USED HERE ARE EITHER NOT USED ANYWHERE ELSE EXCEPT DURING CONSTRUCTION,
  // OR THEY ARE MUTEXED. THIS IS FOR THREAD SAFETY;
  
  // determine the PWM pulse duration
  std::chrono::time_point<std::chrono::steady_clock> point = d_clock.now(); // measure as soon as possibled
  bool new_state = d_interrupt->readPin();
  if (new_state) // start of pulse
    {
      d_pulse_start_pt = point;
    }
  else // end of pulse
    {
      d_pulse_durations.emplaceBack(point - d_pulse_start_pt);
      d_update_timer.singleShotZero();
    }
}

//----------------------------------------------------------------------//
template <class T>
void PwmReader<T>::ownerError(Error e, const std::string& msg)
{
  d_owner.call(&PwmReader::handleError, this, e, msg);
}
