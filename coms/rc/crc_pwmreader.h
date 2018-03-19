#ifndef CRC_PWMREADER_H
#define CRC_PWMREADER_H

#include "core_owner.h"
#include "core_threadpipe.h"

#include "crc_pwmutils.h"

#include "kn_safecallbacktimer.h"
#include "wp_interrupt.h"

#include <chrono>

namespace C_RC
{
  enum class PwmReaderError
      {
	InternalInterruptSetup
	  };
  
  template <class T>
    class PwmReader final : P_WP::Interrupt::Owner
    {
    public:
      class Owner
      {
	OWNER_SPECIAL_MEMBERS(PwmReader);
	// This will only be called when a new duration has been measured.
	virtual void handleValue(PwmReader*, T) = 0;
	virtual void handleValueOutOfRange(PwmReader*, T) = 0;
	virtual void handleError(PwmReader*, PwmReaderError, const std::string& msg) = 0;
      };
    
    public:
      PwmReader(Owner* o,
		P_WP::PinNum pin,
		PwmLimits<T> limits);
      PwmReader(PwmReader&&) = delete;

      SET_OWNER();
      void capData() { d_cap_data = true; }

    
      T getMaxVal() { return d_map.getPwmLimits().max_val; }
      T getMinVal() { return d_map.getPwmLimits().min_val; }
      T getValRange() { return d_map.getPwmLimits().val_range; }
    
      const PwmMap<T>& getMap() { return d_map; }

    private:
      void handleInterrupt(P_WP::Interrupt* interrupt) override;
    
    private:
      void processData();
      std::chrono::nanoseconds getFilteredDuration();
      void ownerPulseDuration(const std::chrono::microseconds& dur);
      void ownerError(PwmReaderError e, const std::string& msg);

    private:
      struct InterruptVals
      {
	InterruptVals() = default;
      InterruptVals(std::chrono::time_point<std::chrono::high_resolution_clock> t_point,
		    bool state)
      : time_point(t_point),
	  pin_state(state)
	{}
      
	std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
	bool pin_state = false;
      };
    
    private:
      CORE::Owner<PwmReader::Owner> d_owner;
      std::unique_ptr<P_WP::Interrupt> d_interrupt;

      PwmMap<T> d_map;
    
      CORE::ThreadPipe<InterruptVals> d_interrupt_vals;
      std::chrono::high_resolution_clock d_clock;
      std::chrono::time_point<std::chrono::high_resolution_clock> d_pulse_start_pt;

      std::vector<std::chrono::nanoseconds> d_durations_buffer;
    
      bool d_had_first_interrupt = false;
      bool d_prev_pin_state = false;
      bool d_cap_data = false;

      KERN::SafeCallbackTimer d_owner_timer = KERN::SafeCallbackTimer("C_RC::PwmReader Owner Timer");
      KERN::SafeCallbackTimer d_error_timer = KERN::SafeCallbackTimer("C_RC::PwmReader Error Timer");
    };

  //----------------------------------------------------------------------//
  template <class T>
    PwmReader<T>::PwmReader(Owner* o,
			    P_WP::PinNum pin,
			    PwmLimits<T> limits)
    : d_owner(o),
    d_map(std::move(limits))
      {
	// This thread-safe timer is used to call commands in the main thread by using a zero time.
	d_owner_timer.setTimeoutCallback([this](){
	    processData();
	  });
	
	// setup interrupt
	d_interrupt.reset(new P_WP::Interrupt(this,
					      pin,
					      P_WP::Interrupt::Mode::Both));

	if (d_interrupt->hasError())
	  {
	    d_error_timer.singleShotZero([this](){
		ownerError(PwmReaderError::InternalInterruptSetup,
			   "PwmReader::PwmReader - Failed to setup the interrupt.");
	      });
	  }
      }

  //----------------------------------------------------------------------//
  template <class T>
    void PwmReader<T>::handleInterrupt(P_WP::Interrupt* interrupt)
    {
      // MAKE SURE THE PRIVATE MEMBERS USED HERE ARE EITHER NOT USED ANYWHERE ELSE EXCEPT DURING CONSTRUCTION,
      // OR THEY ARE MUTEXED. THIS IS FOR THREAD SAFETY;
  
      // Pass measured values ASAP
      std::chrono::time_point<std::chrono::high_resolution_clock> point = d_clock.now(); // measure as soon as possible
      d_interrupt_vals.emplaceBack(std::move(point),interrupt->readPin());
      if (!d_owner_timer.isScheduledToExpire())
	{
	  d_owner_timer.singleShotZero();
	}
    }
  
  //----------------------------------------------------------------------//
  template <class T>
    void PwmReader<T>::processData()
    {
      while (!d_interrupt_vals.empty())
	{
	  InterruptVals vals;
	  if (!d_interrupt_vals.tryPopFront(vals))
	    {
	      assert(false);
	      return;
	    }
      
	  if (!d_had_first_interrupt)
	    {
	      d_had_first_interrupt = true;
	      d_prev_pin_state = vals.pin_state;
	      return;
	    }

	  // avoid duplicate detections
	  if (vals.pin_state == d_prev_pin_state)
	    {
	      return;
	    }

	  // determine the PWM pulse duration
	  if (vals.pin_state) // start of pulse
	    {
	      d_pulse_start_pt = vals.time_point;
	    }
	  // implied low state
	  else if (d_prev_pin_state) // end of pulse
	    {
	      std::chrono::nanoseconds dur_raw = vals.time_point - d_pulse_start_pt;
	      d_durations_buffer.push_back(std::move(dur_raw));
	      ownerPulseDuration(std::chrono::duration_cast<std::chrono::microseconds>(getFilteredDuration()));
	    }
	  d_prev_pin_state = vals.pin_state;
	}
    }
    
  //----------------------------------------------------------------------//
  template <class T>
    std::chrono::nanoseconds PwmReader<T>::getFilteredDuration()
    {
      // Using a median filter
      int diff = static_cast<int>(d_durations_buffer.size()) - 3;
      if (diff > 0)
	{
	  assert(d_durations_buffer.size() == 4);
	  d_durations_buffer.erase(d_durations_buffer.begin());
	}
      else if (diff < 0)
	{
	  return d_durations_buffer.back();
	}
      assert(d_durations_buffer.size() == 3);
      std::vector<std::chrono::nanoseconds> sorted_buffer = d_durations_buffer;
      std::sort(sorted_buffer.begin(), sorted_buffer.end());
      return sorted_buffer.at(1);
    }
    
  //----------------------------------------------------------------------//
  template <class T>
    void PwmReader<T>::ownerPulseDuration(const std::chrono::microseconds& dur)
    {
      if (d_cap_data)
	{
	  d_owner.call(&Owner::handleValue,
		       this,
		       d_map.getValue(d_map.capDuration(dur)));
	  return;
	}
      
      if (!d_map.isDurationValid(dur))
	{
	  d_owner.call(&Owner::handleValueOutOfRange, this, d_map.getValue(dur));
	  return;
	}
      d_owner.call(&Owner::handleValue, this, d_map.getValue(dur));
    }

  //----------------------------------------------------------------------//
  template <class T>
    void PwmReader<T>::ownerError(PwmReaderError e, const std::string& msg)
    {
      d_owner.call(&Owner::handleError, this, e, msg);
    }
};

#endif
