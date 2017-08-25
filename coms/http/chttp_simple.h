#ifndef CHTTP_SIMPLE_H
#define CHTTP_SIMPLE_H

#include "kn_timer.h"

#include <curl/curl.h>
#include <string>

/*
  SimpleHttp is based on asynchronous http communication.

	When defining handleResponse, you can convert the body to a string using std::string(body.begin(), body.end())
 */

namespace C_HTTP
{
	enum class SimpleError
	{
		Internal,
		Timeout
	};

	using ResponseCode = unsigned;
	
	class SimpleHttpOwner // inherit privately
	{
	private:
		friend class SimpleHttp;
		virtual void handleFailed(SimpleError) = 0;
		virtual void handleResponse(ResponseCode,
																const std::vector<std::string>& headers,
																const std::vector<char>& body);
	};
	
  class SimpleHttp final : KERN::KernelTimerOwner
  {
  public:
		explicit SimpleHttp(SimpleHttpOwner* o);
		~SimpleHttp();
    SimpleHttp& operator=(const SimpleHttp&) = delete;
    SimpleHttp(const SimpleHttp&) = delete;

		// must be successful before using class
    bool init(long timeout_sec = 30); 

		// example params = "name=daniel&project=curl"
		// Will return false if current message is not complete
    bool get(const std::string& url); 

	private:
		// KERN::KernelTimerOwner
		bool handleTimeOut(const KernelTimer&) override;

	private:
		static size_t respBodyWrite(char *ptr,
																size_t size_mem,
																size_t num_mem,
																void *userdata);
		
		static size_t respHeaderWrite(char *ptr,
																	size_t size_mem,
																	size_t num_mem,
																	void *userdata);

		static int timerRestart(CURLM *multi,
														long timeout_ms,
														void *userdata);
			
  private:
		SimpleHttpOwner d_owner = nullptr;

    CURL *d_curl = nullptr;
		CURLM *d_curl_multi = nullptr;
    CURLcode d_res_multi = CURLE_OK;

		long d_resp_code = 0;
		std::vector<char> d_resp_body; // could be download data
		std::vector<std::string> d_resp_headers; // http always has header response as text
		
		KERN::KernelTimer d_timer;
  };
};

// Asynchronous implementation to be done:
// Simple example - https://curl.haxx.se/libcurl/c/multi-single.html
// Call curl_multi_perform on a polling timeout to process socket read and writes
// -> Use latter command to check if number of running handles has changed => transfer completed, then use curl_multi_info_read for info on the result.
// Set callback for timeout value using CURLMOPT_TIMERFUNCTION to set the poll time of the call to curl_multi_perform

// Consider handling the body of received data by setting a callback function using the option CURLOPT_WRITEFUNCTION (also see CURLOPT_WRITEDATA)
// Consider using CURLOPT_HEADER to include received header data in received body data

// multi_perform command: https://curl.haxx.se/libcurl/c/curl_multi_perform.html
// curl_multi_info_read: https://curl.haxx.se/libcurl/c/curl_multi_info_read.html

// if version 7.2 then look at https://curl.haxx.se/libcurl/c/curl_multi_info_read.html

#endif
