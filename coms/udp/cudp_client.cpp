#include "cudp_client.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <chrono>
#include <iostream>

using namespace C_UDP;
using namespace boost::asio::ip;

//----------------------------------------------------------------------//
Client::Client(Owner* o,
	       const std::string host,
	       const std::string service)
  : d_owner(o),
    d_host(std::move(host)),
    d_service(std::move(service)),
    d_socket(KERN::AsioKernel::getService())
{
  d_reconnect_timer.setTimeoutCallback([this](){
      connect();
    });
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
void Client::connect()
{
  if (d_socket.is_open())
    {
      return;
    }
  
  try
    {
      udp::resolver resolver(KERN::AsioKernel::getService());
      udp::resolver::query query(d_host, d_service);

      // attempt to connect to each endpoint in the resolver list until successful (includes v4 and v6 ip protocols)
      boost::asio::connect(d_socket, resolver.resolve(query));
    }
  catch (std::exception& e)
    {
      std::cerr << "Client::connect() - Failed. Error is " << e.what() << " " << std::endl;
      // LOG
      d_owner->handleFailed(this, Error::Connect);
      return;
    }
  d_owner->handleConnected(this);
}

//----------------------------------------------------------------------//
void Client::connect(std::chrono::milliseconds delay)
{
  connect();
  if (!d_socket.is_open())
    {
      d_reconnect_timer.restart(delay);
    }
}

//----------------------------------------------------------------------//
void Client::send(const void* data, size_t byte_size)
{
  if (d_is_sending)
    {
      assert(false);
      d_owner->handleFailed(this, Error::AlreadySending);
      return;
    }
  d_is_sending = true;
  sendInternal(data, byte_size);
}

//----------------------------------------------------------------------//
void Client::sendInternal(const void* data, size_t byte_size)
{
  d_transfer_data = static_cast<const uint8_t*>(data);
  d_transfer_data_size = byte_size;
  d_socket.async_send(boost::asio::buffer(data, byte_size),
		      [=](const boost::system::error_code& err,
			  size_t bytes_transferred){
			if (err.value() != boost::system::errc::success)
			  {
			    std::cerr << "Client::send - error message: " << err.message() << std::endl;
			    
			    if (d_socket.is_open())
			      {
				d_socket.close();
			      }
			    
			    d_owner->handleFailed(this, Error::Disconnected);
			    return;
			  }
			
			if (bytes_transferred != d_transfer_data_size)
			  {
			    sendInternal(d_transfer_data + bytes_transferred,
					 d_transfer_data_size - bytes_transferred);
			    return;
			  }
			// else successful
			d_is_sending = false;
			d_owner->handleMessageSent(this);
		      });
}
