#ifndef CHTTP_SIMPLEHTTP_H
#define CHTTP_SIMPLEHTTP_H

#include <curl/curl.h>
#include <string>

/*
  SimpleHttp is based on synchronous http communication.
 */

namespace C_HTTP
{
  class SimpleHttp final
  {
  public:
    SimpleHttp() = default;
    ~SimpleHttp();
    SimpleHttp& operator=(const SimpleHttp&) = delete;
    SimpleHttp(const SimpleHttp&) = delete;

    bool init(long timeout_sec = 2); //must be successful before using class
    void setCompleteCallback(std::function);
    bool get(const std::string& url); //example params = "name=daniel&project=curl"
    std::string getError();
    
  private:
    CURL *d_curl = nullptr;
		CURLM *d_curl_multi = nullptr;
    CURLcode d_res = CURLE_OK;
    bool d_ready = false;
  };
};

// Asynchronous implementation to be done:
// Simple example - https://curl.haxx.se/libcurl/c/multi-single.html
// Call curl_multi_perform on a polling timeout to process socket read and writes
// -> Use latter command to check if number of running handles has changed => transfer completed, then use curl_multi_info_read for info on the result.
// Set callback for timeout value using CURLMOPT_TIMERFUNCTION to set the poll time of the call to curl_multi_perform

// Consider handling the body of received data by setting a callback function using the option CURLOPT_WRITEFUNCTION (also see CURLOPT_WRITEDATA)
// Consider handling the header of received data by
// Consider using CURLOPT_HEADER to include received header data in received body data

// multi_perform command: https://curl.haxx.se/libcurl/c/curl_multi_perform.html
// curl_multi_info_read: https://curl.haxx.se/libcurl/c/curl_multi_info_read.html

#endif
