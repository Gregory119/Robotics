#ifndef CRC_PPMREADER_H
#define CRC_PPMREADER_H

#include "core_observer.h"
#include "crc_pwmutils.h"
#include "kn_asiocallbacktimer.h"
#include "utl_medianfilter.h"
#include "wp_syncedinterrupt.h"

#include <chrono>
#include <list>
#include <map>
#include <unordered_map>

namespace C_RC
{
  enum class PpmReaderError
  {
    InternalInterrupt,
      InvalidDuration
      };

  unsigned getMaxPpmChannels();
  
  // Multiple observers can exist with the same channel.
  template <class T>
    class PpmReader final : P_WP::SyncedInterrupt::Owner
    {
    public:
      class Observer
      {
	OBSERVER_SPECIAL_MEMBERS(PpmReader);
	// this will only be called when a new duration has been measured
	virtual void handleValue(PpmReader*, T) = 0;
	virtual void handleValueOutOfRange(PpmReader*, T) = 0;
	virtual void handleError(PpmReader*,
				 PpmReaderError,
				 const std::string& msg) = 0;
	
	// The channel value should never change and it should be greater than zero.
	virtual unsigned getChannel() const = 0;
      };
    
    public:
      PpmReader(P_WP::PinNum, const PwmLimits<T>&);
      PpmReader(PpmReader&&) = delete;

      bool attachObserver(Observer*);
      void clearObservers() { d_observers.clear(); }

      void start();
      void stop();
      
      void capData() { d_cap_data = true; }

      T getMaxVal() { return d_map.getPwmLimits().max_val; }
      T getMinVal() { return d_map.getPwmLimits().min_val; }
      T getValRange() { return d_map.getPwmLimits().val_range; }
      
      // This will only fail if the number of channels has not been detected yet.
      bool getNumChannels(unsigned&);
      
    private:
      void handleInterrupt(P_WP::SyncedInterrupt*,
			   P_WP::Interrupt::Vals&&) override;
      void handleError(P_WP::SyncedInterrupt*,
		       P_WP::Interrupt::Error,
		       const std::string&) override;
      
    private:
      struct Duration
      {
	Duration() = default;
      Duration(std::chrono::microseconds dur, unsigned ch)
      :   duration(std::move(dur)),
	  channel(ch)
	{}
	std::chrono::microseconds duration;
	unsigned channel = 0;
      };

    private:
      void processAllChannelData();
      std::chrono::microseconds getFilteredDuration(const Duration&);
      void observerValue();
      void clearData();
      void observerError(PpmReaderError, const std::string& msg);
    
    private:
      std::multimap<unsigned, Observer*> d_observers;

      bool d_cap_data = false;
      
      std::unique_ptr<P_WP::SyncedInterrupt> d_interrupt;
      
      PwmMap<T> d_map;
      unsigned d_channels = 0; // will be auto-detected
      unsigned d_channel_count = 0; // zero represents the start pulse
      unsigned d_prev_channel_count = 0; // zero represents the start pulse

      std::list<Duration> d_all_channel_durs;
      std::unordered_map<unsigned,UTIL::MedianFilter<std::chrono::microseconds>> d_channel_filters;
      std::chrono::time_point<std::chrono::steady_clock> d_pulse_start_pt;

      KERN::AsioCallbackTimer d_timer;
    };

  //----------------------------------------------------------------------//
  template <class T>
    PpmReader<T>::PpmReader(P_WP::PinNum pin,
			    const PwmLimits<T>& limits)
    : d_map(limits)
    {
      if (!std::is_fundamental<T>::value)
	{
	  assert(false);
	  //LOG!!!
	  return;
	}

      d_timer.setTimeoutCallback([this](){
	  processAllChannelData();
	});
      
      d_interrupt.reset(new P_WP::SyncedInterrupt(this,
						  pin,
						  P_WP::Interrupt::EdgeMode::Rising));
    }

  //----------------------------------------------------------------------//
  template <class T>
    void PpmReader<T>::start()
    {
      d_interrupt->start();
    }

  //----------------------------------------------------------------------//
  template <class T>
    void PpmReader<T>::stop()
    {
      d_interrupt->stop();
    }
  
  //----------------------------------------------------------------------//
  template <class T>
    bool PpmReader<T>::attachObserver(Observer* ob)		
    {						
      if (ob == nullptr)				
	{						
	  assert(false);				
	  return false;				
	}

      // the pointer should not be in the observer list
      for (const auto& pair : d_observers)
	{
	  if (pair.second == ob)
	    {
	      assert(false);
	      return false;
	    }
	}

      d_observers.insert(std::pair<unsigned, Observer*>(ob->getChannel(), ob));
      return false;
    }
      
  //----------------------------------------------------------------------//
  template <class T>
    bool PpmReader<T>::getNumChannels(unsigned& channels)
    {
      if (d_channels == 0)
	{
	  return false;
	}
      channels = d_channels;
      return true;
    }
      
  //----------------------------------------------------------------------//
  template <class T>
    void PpmReader<T>::processAllChannelData()
    {
      // Check if the number of detected channels has changed.
      if (d_channel_count != d_channels)
	{
	  if (d_channel_count != d_prev_channel_count)
	    {
	      d_all_channel_durs.clear();
	      d_prev_channel_count = d_channel_count;
	      d_channel_count = 0;
	      return;
	    }

	  // LOG!!!!: the number of data channels has changed
	  d_channels = d_channel_count;
	}

      // send data to observers
      for (unsigned i=0; i<d_channel_count; ++i)
	{
	  observerValue();
	}
      d_channel_count = 0;
      assert(d_all_channel_durs.empty());
    }

  //----------------------------------------------------------------------//
  template <class T>
    std::chrono::microseconds
    PpmReader<T>::getFilteredDuration(const Duration& dur)
    {
      try
	{
	  d_channel_filters.at(dur.channel);
	}
      catch(...)
	{
	  d_channel_filters[dur.channel] = UTIL::MedianFilter<std::chrono::microseconds>(4);
	}
      UTIL::MedianFilter<std::chrono::microseconds>& filter = d_channel_filters[dur.channel];
      filter.pushBack(dur.duration);
      return filter.getMedian();
    }
  
  //----------------------------------------------------------------------//
  template <class T>
    void PpmReader<T>::observerValue()
    {
      if (d_all_channel_durs.empty())
	{
	  return;
	}
      
      Duration dur = d_all_channel_durs.front();
      d_all_channel_durs.pop_front();

      if (d_observers.empty())
	{
	  return;
	}

      std::chrono::microseconds dur_filtered = getFilteredDuration(dur);
      auto obs_range = d_observers.equal_range(dur.channel);
      // cap value
      if (d_cap_data)
	{
	  std::chrono::microseconds dur_filt_capped;
	  if (!d_map.capDuration(dur_filtered, dur_filt_capped)) // cache for multiple observers with the same channel
	    {
	      return;
	    }
	  for (auto i=obs_range.first; i!=obs_range.second; ++i)
	    {
	      i->second->handleValue(this, d_map.getValue(dur_filt_capped));
	    }
	  return;
	}
	  
      // out of range
      if (!d_map.isDurationValid(dur_filtered))
	{
	  unsigned val = d_map.getValue(dur_filtered); 
	  for (auto i=obs_range.first; i!=obs_range.second; ++i)
	    {
	      i->second->handleValueOutOfRange(this, val);
	    }
	  return;
	}

      // in range
      unsigned val = d_map.getValue(dur_filtered); 
      for (auto i=obs_range.first; i!=obs_range.second; ++i)
	{
	  i->second->handleValue(this, val);
	}
    }

  //----------------------------------------------------------------------//
  template <class T>
  void PpmReader<T>::handleInterrupt(P_WP::SyncedInterrupt*,
				     P_WP::Interrupt::Vals&& vals)
    {
      // detect start edge
      if (d_channel_count == 0)
	{
	  d_pulse_start_pt = vals.time_point;
	  ++d_channel_count;
	  return;
	}
      
      // => d_channel_count > 0
      // add the new single channel data
      d_all_channel_durs.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(vals.time_point - d_pulse_start_pt),
				      d_channel_count);
      d_timer.singleShot(std::chrono::microseconds(d_map.getPwmLimits().max_duration_micros*115/100));

      d_pulse_start_pt = vals.time_point; // for next channel
      ++d_channel_count;
    }

  //----------------------------------------------------------------------//
  template <class T>
    void PpmReader<T>::handleError(P_WP::SyncedInterrupt*,
				   P_WP::Interrupt::Error e,
				   const std::string& msg)
    {
      std::string new_msg = "PpmReader::PpmReader - Interrupt failure with the message:\n";
      new_msg += msg;
      observerError(PpmReaderError::InternalInterrupt,
		    new_msg);
    }

  //----------------------------------------------------------------------//
  template <class T>
    void PpmReader<T>::observerError(PpmReaderError e,
				     const std::string& msg)
    {
      for (const auto& ob : d_observers)
	{
	  ob.second->handleError(this, e, msg);
	}
    }
};

#endif
