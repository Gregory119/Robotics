#ifndef CHTTP_OPERATIONS_H
#define CHTTP_OPERATIONS_H

#include "kn_timer.h"

#include <curl/curl.h>
#include <string>
#include <vector>
#include <string>
#include <list>

/*
  HttpOperations is based on asynchronous http communication.

  When defining handleResponse, you can convert the body to a string using std::string(body.begin(), body.end()).
  When adding more functions, MAKE SURE TO CHECK READY FLAG AT THE START.
*/

namespace C_HTTP
{
  enum ResponseCode
  {
    Continue = 100, // 1xx informational
    Ok = 200, // 2xx success
    MultipleChoices = 300, // 3xx redirections
    BadRequest = 400, // 4xx client errors
    InternalServerError = 500 // 5xx server errors
  };
  
  enum class HttpOpError
  {
    Internal, // this should not happen
    Timeout
  };

  using HttpResponseCode = int;

  class HttpOperations;
  class HttpOperationsOwner // inherit privately
  {
  private:
    friend class HttpOperations;
    virtual void handleFailed(HttpOperations*,HttpOpError) = 0;
    virtual void handleResponse(HttpOperations*,
				HttpResponseCode,
				const std::vector<std::string>& headers,
				const std::vector<char>& body) = 0;
  };
	
  class HttpOperations final : KERN::KernelTimerOwner
  {
  public:
    explicit HttpOperations(HttpOperationsOwner* o);
    ~HttpOperations();
    HttpOperations& operator=(const HttpOperations&) = delete;
    HttpOperations(const HttpOperations&) = delete;

    // Must be successful before using class.
    // Will return true if already initialized.
    bool init(long timeout_sec = 30); 

    // example params = "name=daniel&project=curl"
    // Will return false if current message is not complete
    void get(const std::string& url); 
    std::string getUrl() { return d_url; }

    void cancelBufferedReqs();
    bool hasBufferedReqs() { return !d_reqs.empty(); }
    
  private:
    // KERN::KernelTimerOwner
    bool handleTimeOut(const KERN::KernelTimer&) override;

    // Curl callbacks
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
    void processMessage();
    void processNextBufferedReq();

  private:
    enum class RequestType
    {
      Get,
      Post
    };
    
    struct Request
    {
      RequestType type = RequestType::Get;
      std::string url;
      std::string body;
    };
    
  private:
    HttpOperationsOwner *d_owner = nullptr;

    CURL *d_curl = nullptr;
    CURLM *d_curl_multi = nullptr;

    std::string d_url;

    bool d_ready = false;
    long d_resp_code = 0;
    std::vector<char> d_resp_body; // could be download data
    std::vector<std::string> d_resp_headers; // http always has header response as text
    int d_running_transfers = 0;
		
    KERN::KernelTimer d_timer_process = KERN::KernelTimer(this);
    KERN::KernelTimer d_timer_failed_init = KERN::KernelTimer(this);

    std::list<Request> d_reqs;
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
