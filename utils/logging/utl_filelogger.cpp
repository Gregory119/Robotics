#include "utl_filelogger.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

std::array<const char*, 6> s_strings =
  {
    "DEBUG",
    "NORM",
    "INFO",
    "WARN",
    "ERROR",
    "CRIT"
  };

using namespace UTIL;

struct SeverityTag;

//----------------------------------------------------------------------//
// This operator is used for regular stream formatting
std::ostream& operator<<(std::ostream& strm, SeverityLevel level)
{
  try
    {
      strm << s_strings.at(static_cast<int>(level));
    }
  catch (...)
    {
      assert(false);
      strm << static_cast<int>(level);
    }

  return strm;
}

//----------------------------------------------------------------------//
// The operator is used when putting the severity level to log
boost::log::formatting_ostream& operator<<(boost::log::formatting_ostream& strm,
					   boost::log::to_log_manip<SeverityLevel,
					   SeverityTag > const& manip)
{
  SeverityLevel level = manip.get();
  try
    {
      strm << s_strings.at(static_cast<int>(level));
    }
  catch (...)
    {
      assert(false);
      strm << static_cast<int>(level);
    }

  return strm;
}

//----------------------------------------------------------------------//
FileLogger::FileLogger(const Params& params)
{
  namespace logging = boost::log;
  namespace src = logging::sources;
  namespace sinks = logging::sinks;
  namespace keywords = logging::keywords;
  namespace expr = logging::expressions;
  using file_sink = sinks::text_file_backend;
  using sink_t = sinks::synchronous_sink<file_sink>;
  
  try
    {
      boost::shared_ptr<logging::core> core = logging::core::get();

      std::string file_name;
      if (!params.file_name.empty())
	{
	  file_name = params.file_name;
	  file_name += "_%5N.log";
	}
      else
	{
	  file_name += "%5N.log";
	}
      
      boost::shared_ptr<file_sink> backend = 
	boost::make_shared<file_sink>(keywords::file_name = file_name,
				      keywords::open_mode = std::ios_base::app,
				      keywords::rotation_size = params.rotation_size_kb * 1024,
				      keywords::enable_final_rotation = false, // Do not increment log file count on destruction. This avoids a bug with the Boost file system.
				      keywords::auto_flush = true); // Ensure last log line is written before a crash.

      // Wrap it into the frontend and register in the core.
      // The backend requires synchronization in the frontend.
      boost::shared_ptr<sink_t> d_sink(new sink_t(backend));

      auto format_with_thread = expr::format("[%1%] [%2%] [%3%] %4% : %5%")
	% expr::attr<boost::posix_time::ptime>("TimeStamp")
	% expr::attr<SeverityLevel, SeverityTag>("Severity")
	% expr::attr<unsigned int>("ThreadID")
	% expr::attr<unsigned int>("LineID")
	% expr::smessage;
      
      auto format_no_thread = expr::format("[%1%] [%2%] %3% : %4%")
	% expr::attr<boost::posix_time::ptime>("TimeStamp")
	% expr::attr<SeverityLevel, SeverityTag>("Severity")
	% expr::attr<unsigned int>("LineID")
	% expr::smessage;

      if (params.enable_thread_id)
	{
	  d_sink->set_formatter(format_with_thread);
	}
      else
	{
	  d_sink->set_formatter(format_no_thread);
	}
  
      d_sink->locked_backend()->
	set_file_collector(sinks::file::make_collector(keywords::target = params.rotation_dir, // folder containing logs
						       keywords::max_files = params.max_rotated_files));
  
      // Scan files to update list of previous logs for possible deletion on rotation
      // The counter will start from 0 every time the logging is created
      d_sink->locked_backend()->scan_for_files();

#ifdef RELEASE
      d_sink->set_filter(expr::attr<SeverityLevel>("Severity") >=
			 SeverityLevel::NORM);
#else
      d_sink->set_filter(expr::attr<SeverityLevel>("Severity") >=
			 SeverityLevel::DEBUG);
      if (params.enable_thread_id)
	{
	  logging::add_console_log(std::clog,
				   keywords::format = format_with_thread);
	}
      else
	{
	  logging::add_console_log(std::clog,
				   keywords::format = format_no_thread);
	}
#endif
      
      core->add_sink(d_sink);
  
      logging::add_common_attributes();
    }
  catch (...)
    {
      d_has_error = true;
    }
}

//----------------------------------------------------------------------//
FileLogger::~FileLogger()
{
  boost::shared_ptr<boost::log::core> core = boost::log::core::get();
  core->remove_sink(d_sink); // no effect if the sink was not previously registered
}
