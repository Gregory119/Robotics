#ifndef CUDP_CLIENT
#define CUDP_CLIENT

#include <kn_asiocallbacktimer.h>

#include <boost/asio.hpp>
#include <vector>

namespace C_UDP
{
  class Client final
  {
  public:
    enum class Error
    {
      Construction,
      Unknown
    };
    
    class Owner
    {
    protected:
      Owner() = default;
      ~Owner() = default;
      Owner(const Owner&) = delete;
      Owner& operator=(const Owner&) = delete;
      Owner(const Owner&&) = delete;
      Owner& operator=(const Owner&&) = delete;

    private:
      friend Client;
      void handleMessageSent();
      //void handleReceivedMessage(const std::vector<char>&);
      void handleFailed(Error);
    };

    // host - ip or host name
    // service - name or port number or empty for a zero port number
    Client(Owner*,const std::string& host, const std::string& service);
    ~Client();
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    // move constructors are implicitly not declared => will not compile if attempted to use (check this)
    
    send(const void* data, size_t byte_size); // to the host (ip) with service (port)
    //receive(); // from the host
    
  private:
    Owner* d_owner = nullptr;

    void* d_transfer_data = nullptr;
    size_t d_transfer_data_size = 0;
    
    boost::asio::ip::udp::endpoint d_endpoint;
    boost::asio::ip::udp::socket d_socket;

    KERN::AsioCallbackTimer d_timer;
  };
};

#endif
