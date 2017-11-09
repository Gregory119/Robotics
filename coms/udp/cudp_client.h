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
      AlreadySending,
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
      virtual void handleMessageSent(Client*) = 0;
      //virtual void handleReceivedMessage(const std::vector<char>&);
      virtual void handleFailed(Client*, Error) = 0;
    };

    // host - ip or host name
    // service - name or port number or empty for a zero port number
    // these strings can be temporaries
    Client(Owner*,const std::string& host, const std::string& service);
    ~Client();
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    Client(const Client&&) = delete;
    Client& operator=(const Client&&) = delete;
    
    void send(const void* data, size_t byte_size); // to the host (ip) with service (port)
    //receive(); // from the host
    bool isSending() { return d_is_sending; }

  private:
    void sendInternal(const void* data, size_t byte_size);
    
  private:
    Owner* d_owner = nullptr;

    const uint8_t* d_transfer_data = nullptr;
    size_t d_transfer_data_size = 0;
    
    boost::asio::ip::udp::endpoint d_endpoint;
    boost::asio::ip::udp::socket d_socket;

    KERN::AsioCallbackTimer d_timer;

    bool d_is_sending = false;
  };
};

#endif
