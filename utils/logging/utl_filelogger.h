#ifndef UTL_FILELOGGER_H
#define UTL_FILELOGGER_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

enum SeverityLevel
{
  DEBUG,
    NORM,
    INFO,
    WARN,
    ERROR,
    CRIT
};

namespace UTIL
{
  // log format: "[TimeStamp] [Severity] [ThreadID] LineID : Message"
  // The ThreadID is optionally included.
  
  class FileLogger final
  {
  public:
    struct Params
    {
      std::string file_name; // The name can include the path. It will be appended with the rotation number. If it is empty, then the rotation number is used as the name.
      std::string rotation_dir = "previous_logs"; // Where the rotated log files are automatically stored by FileLogger. This directory should not be in the file name. This rotation directory is created if not found.
      int rotation_size_kb = 1024; // [KiB] the log will rotate if its size exceeds this
      int max_rotated_files = 10;
      bool enable_thread_id = false;
    };
    
  public:
    FileLogger(const Params& = {}); 
    ~FileLogger();

    // This function must be called once after construction
    bool hasError() { return d_has_error; }

    template <class T>
      void log(SeverityLevel, T&& value);
    
  private:
    bool d_has_error = false;
    boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>> d_sink;
    boost::log::sources::severity_logger<SeverityLevel> d_slg;
  };

  //----------------------------------------------------------------------//
  template <class T>
    void FileLogger::log(SeverityLevel level, T&& value)
    {
      if (d_has_error)
	{
	  assert(false);
	  return;
	}
  
      BOOST_LOG_SEV(d_slg, level) << "\n" << std::forward<T>(value) << "\n";
    }
};

#endif
