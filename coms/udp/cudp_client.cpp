#include "cudp_client.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <chrono>
#include <iostream>

using namespace C_UDP;
using namespace boost::asio::ip;

//----------------------------------------------------------------------//
Client::Client(Owner* o,
	       const std::string& host,
	       const std::string& service)
  : d_owner(o),
    d_socket(AsioKernel::getService())
{
  d_timer.setCallback([this](){
      d_owner->handleFailed(Error::Construction);
    });
  
  try
    {
      udp::resolver resolver(AsioKernel::getService());
      udp::resolver::query query(host, service);

      // attempt to connect to each endpoint in the resolver list until successful (includes v4 and v6 ip protocols)
      boost::asio::connect(d_socket, resolver.resolve());
    }
  catch (...)
    {
      // LOG
      assert(false);
      d_timer.singleShot(std::chrono::seconds(0));

      if (d_socket.is_open())
	{
	  d_socket.close();
	}
    }
}

//----------------------------------------------------------------------//
Client::~Client()
{
  if (d_socket.is_open())
    {
      d_socket.close();
    }
}

//----------------------------------------------------------------------//
Client::send(const void* data, size_t byte_size)
{
  d_transfer_data = data;
  d_transfer_data_size = byte_size;
  d_socket.async_send(boost::asio::buffer(data, byte_size),
		      [=](const boost::system::error_code& err,
			  size_t bytes_transferred){
			if (err.value() != boost::system::errc::success)
			  {
			    assert(false);
			    std::cerr << "Client::send - error message: " << err.message() << std::endl;
			    d_owner->handleFailed(Error::Unknown);
			    return;
			  }
			
			if (bytes_transferred != byte_size)
			  {
			    send(d_transfer_data + bytes_transferred,
				 d_transfer_data_size - bytes_transferred);
			    return;
			  }
			// else successful
			d_owner->handleMessageSent();
		      });
}
