#ifndef KN_ASIOKERNEL_H
#define KN_ASIOKERNEL_H

#include <boost/asio.hpp>

namespace KERN
{
  class AsioCallbackTimer;
  class AsioKernel 
  {
    // A service that currently only processes timer timeouts. It uses a thread_local singleton (one instance per thread).
  public:
    AsioKernel();
    virtual ~AsioKernel() = default;
    AsioKernel& operator=(const AsioKernel&) = delete;
    AsioKernel(const AsioKernel&) = delete;
    AsioKernel& operator=(AsioKernel&&) = delete;
    AsioKernel(AsioKernel&&) = delete;    
    
    void run();
    static boost::asio::io_service& getService();

  private:
    void keepBusy(const boost::system::error_code& err,
		  boost::asio::deadline_timer& t);

  private:
    boost::asio::io_service d_io;
    boost::asio::deadline_timer d_timer;
  };
};

#endif
