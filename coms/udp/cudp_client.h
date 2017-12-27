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
      Connect,
      AlreadySending,
      Disconnected,
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
      virtual void handleConnected(Client*) = 0;
      virtual void handleMessageSent(Client*) = 0;
      //virtual void handleReceivedMessage(const std::vector<char>&);
      virtual void handleFailed(Client*, Error) = 0;
    };

    // host - ip or host name
    // service - name or port number or empty for a zero port number
    // these strings can be temporaries
    Client(Owner*,const std::string host, const std::string service);
    ~Client();
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    Client(const Client&&) = delete;
    Client& operator=(const Client&&) = delete;

    // The class must be connected, with any of the connect functions, in order to be used.
    // connect() can be called multiple times, but will only try to connect if not connected.
    void connect();

    // connect(std::chrono::milliseconds) will first try to connect and then continue to
    // try to connect on an interval, with the specified delay, until it is connected.
    // The connect failure callbacks are still called.
    void connect(std::chrono::milliseconds);
    
    void send(const void* data, size_t byte_size); // to the host (ip) with service (port)
    //receive(); // from the host
    bool isSending() { return d_is_sending; }

  private:
    void sendInternal(const void* data, size_t byte_size);
    
  private:
    Owner* d_owner = nullptr;
    std::string d_host;
    std::string d_service;
    
    boost::asio::ip::udp::socket d_socket;
    boost::asio::ip::udp::endpoint d_endpoint;

    const uint8_t* d_transfer_data = nullptr;
    size_t d_transfer_data_size = 0;

    bool d_is_sending = false;
    bool d_is_connected = false;

    KERN::AsioCallbackTimer d_reconnect_timer = KERN::AsioCallbackTimer("C_UDP::Client - reconnect timer.");
  };
};

#endif
