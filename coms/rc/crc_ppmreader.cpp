#include "crc_ppmreader.h"

using namespace C_RC;

//----------------------------------------------------------------------//
template <class T>
PpmReader<T>::PpmReader(Owner* o,
			P_WP::PinNum pin,
			PwmLimits<T> limits)
  : d_owner(o),
    d_map(std::move(limits))
{
  // setup interrupt
  d_interrupt.reset(new P_WP::Interrupt(pin,
					P_WP::Interrupt::Mode::Rising,
					[this](){
					  processInterrupt();
					}));

  if (d_interrupt->hasError())
    {
      d_error_timer.singleShotZero([this](){
	  d_owner.call(PpmReader::handleError,
		       Error::InternalInterruptSetup,
		       "PpmReader::PpmReader - Failed to setup the interrupt.");
	});
    }

  // This thread-safe timer is used to call commands in the main thread by using a zero time.
  d_data_timer.setTimeoutCallback([this](){
      std::lock_guard<std::mutex> lk(d_data_mutex);
      processData();
    });
}

//----------------------------------------------------------------------//
template <class T>
void PpmReader<T>::processData()
{
  // Check if the number of detected channels has changed.
  if (d_current_channel != d_channels)
    {
      ++d_count_conseq_diff_channels;
      if (d_count_conseq_diff_channels < 2)
	{
	  d_pulse_durations.clear();
	  return;
	}

      // LOG: the number of data channels has changed
      d_channels = d_current_channel;
    }

  d_count_conseq_diff_channels = 0;
  
  // send data to owner
  for (unsigned i=0; i<d_current_channel; ++i)
    {
      ownerValue();
    }
  d_current_channel = 0;
}

//----------------------------------------------------------------------//
template <class T>
void PpmReader<T>::ownerValue()
{
  Duration dur;
  if (!d_pulse_durations.tryPopFront(dur))
    {
      // LOG
      assert(false);
      return;
    }

  if (!d_map.isDurationValid(dur.duration))
    {
      assert(false);
      std::ostringstream stream("PpmReader::ownerValue - The measured pulse duration of ",
				std::ios_base::app);
      stream << dur.duration.count() << " [us] for channel "
	     << dur.channel << " is invalid because it is out of bounds. Max [us] = "
	     << d_map.getPwmLimits().max_duration_micros << ", Min [us] = "
	     << d_map.getPwmLimits().min_duration_micros << ".";
      ownerError(Error::InvalidDuration, stream.str());
      return;
    }
  
  d_owner.call(PpmReader::handleValue,
	       this,
	       {dur.channel, d_map.getValue(dur.duration)});
}

//----------------------------------------------------------------------//
template <class T>
void PpmReader<T>::processInterrupt()
{
  // For thread safety, make sure d_clock is not used anywhere else except during class construction
  std::chrono::time_point<std::chrono::steady_clock> point = d_clock.now(); // measure as soon as possible

  std::lock_guard<std::mutex> lk(d_data_mutex);
  
  // detect start edge
  if (d_current_channel == 0)
    {
      d_pulse_start_pt = point;
      ++d_current_channel;
    }
  // measure channel duration
  else // d_current_channel > 0
    {
      d_pulse_durations.emplaceBack(point - d_pulse_start_pt,
				    d_current_channel);
      d_pulse_start_pt = point; // for next channel
      ++d_current_channel;
    }

  // Start/restart the timer to detect the end of the new channel values
  d_data_timer.singleShot(std::chrono::milliseconds(d_map.getPwmLimits().max_duration_micros*115/100)); // add 15% to longest expected duration
}

//----------------------------------------------------------------------//
template <class T>
void PpmReader<T>::ownerError(Error e, const std::string& msg)
{
  d_owner.call(&PpmReader::handleError, this, e, msg);
}
