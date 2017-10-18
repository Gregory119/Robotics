#include <iostream>
//#include <chrono>
//#include <thread>
#include <boost/asio.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>

void print(const boost::system::error_code& err,
	   boost::asio::deadline_timer* t)
{
  // can check for an error here
  
  t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
  t->async_wait([&](const boost::system::error_code& e){
      print(e,t);
    });
}

int main(int argc, char** argv)
{
  boost::asio::io_service io;
  boost::asio::deadline_timer t(io, boost::posix_time::seconds(1));
  t.async_wait([&](const boost::system::error_code& e){
      print(e,&t);
    });

  io.run();
  
  /* while (1)
    {
      //std::this_thread::sleep_for(std::chrono::seconds(1));
      }*/
  
  return 0;
}
