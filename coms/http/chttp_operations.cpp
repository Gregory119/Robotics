#include "chttp_operations.h"

#include <cassert>
#include <iostream>

using namespace C_HTTP;

//----------------------------------------------------------------------//
HttpOperations::HttpOperations(HttpOperationsOwner* o)
  : d_owner(o)
{
  assert(o != nullptr);
  d_resp_headers.reserve(10*30); // assuming 10 headers each with 30 characters (assuming digest auth)

  d_timer_process.setCallback([this](){
      process();
    });
  d_timer_failed_init.setCallback([this](){
      failedInit();
    });
}

//----------------------------------------------------------------------//
HttpOperations::~HttpOperations()
{
  if ( d_curl != nullptr)
    {
      curl_easy_cleanup(d_curl);
    }

  if (d_curl_multi != nullptr)
    {
      curl_multi_cleanup(d_curl_multi);
    }
  	
  curl_global_cleanup();
}

//----------------------------------------------------------------------//
void HttpOperations::init(std::chrono::seconds timeout)
{
  //std::cout << "HttpOperations::init" << std::endl;
  if (d_is_initialised)
    {
      assert(false);
      return;
    }

  // multiple calls have the same affect as one call
  // NB: not thread safe
  if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) 
    {
      d_timer_failed_init.restartMs(0);
      return;
    }

  if (d_curl != nullptr)
    {
      curl_easy_cleanup(d_curl);
    }
  
  d_curl = curl_easy_init();
  if (d_curl == nullptr)
    {
      d_timer_failed_init.restartMs(0);
      return;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_TIMEOUT, timeout.count()) != CURLE_OK)
    {
      d_timer_failed_init.restartMs(0);
      return;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_WRITEFUNCTION, respBodyWrite) != CURLE_OK)
    {
      d_timer_failed_init.restartMs(0);
      return;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_WRITEDATA, this) != CURLE_OK)
    {
      d_timer_failed_init.restartMs(0);
      return;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_HEADERFUNCTION, respHeaderWrite) != CURLE_OK)
    {
      d_timer_failed_init.restartMs(0);
      return;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_HEADERDATA, this) != CURLE_OK)
    {
      d_timer_failed_init.restartMs(0);
      return;
    }

  d_curl_multi = curl_multi_init();
  if (d_curl_multi == nullptr)
    {
      d_timer_failed_init.restartMs(0);
      return;
    }
	
  if (curl_multi_setopt(d_curl_multi, CURLMOPT_TIMERFUNCTION, timerRestart) != CURLM_OK)
    {
      d_timer_failed_init.restartMs(0);
      return;
    }

  if (curl_multi_setopt(d_curl_multi, CURLMOPT_TIMERDATA, this) != CURLM_OK)
    {
      d_timer_failed_init.restartMs(0);
      return;
    }
	
  d_is_initialised = true;
}

//----------------------------------------------------------------------//
void HttpOperations::get(const std::string& url)
{
  std::cout << "HttpOperations::get - url = " << url << std::endl;
  if (!d_is_initialised)
    {
      assert(false);
      return;
    }
	
  if (d_is_processing_req)
    {
      //std::cout << "HttpOperations::get - buffering url = " << url << std::endl;
      Request req;
      req.type = RequestType::Get;
      req.url = url;
      d_buf_reqs.push_back(req);
      return;
    }

  //std::cout << "HttpOperations::get: starting to process get" << url << std::endl;
  d_is_processing_req = true;
  d_url = url;
  curl_easy_setopt(d_curl, CURLOPT_URL, url.c_str());
  curl_multi_add_handle(d_curl_multi, d_curl);
  
  d_resp_body.clear();
  d_resp_headers.clear();
  d_timer_process.restartMs(0);
}

//----------------------------------------------------------------------//
void HttpOperations::process()
{
  CURLMcode res_multi = curl_multi_perform(d_curl_multi, &d_running_transfers);

  if (res_multi != CURLM_OK)
    {
      // LOG THIS: the exact error details should be logged here
      assert(false); // this should not happen
      d_timer_process.disable();
      d_owner->handleFailed(this,HttpOpError::Internal);
      return;
    }
			
  // has the transfer completed?
  //std::cout << "HttpOperations::process " << "d_running_transfers = " << d_running_transfers << std::endl;
  if (d_running_transfers == 0)
    {
      d_timer_process.disable();
      processMessage();
      processNextBufferedReq(); // next request can start after owner messages (owner may want to clear/stop the buffered messages)
    }		
}

//----------------------------------------------------------------------//
void HttpOperations::failedInit()
{
  d_timer_failed_init.disable();
  d_owner->handleFailed(this,HttpOpError::Internal);
}

//----------------------------------------------------------------------//
void HttpOperations::processNextBufferedReq()
{
  if (d_buf_reqs.empty() ||
      d_is_processing_req) // do not want to move first buffered request to the back of the buffer
    {
      // nothing to do
      return;
    }

  Request& req = d_buf_reqs.front();
  switch (req.type)
    {
    case RequestType::Get:
      get(req.url);
      break;

    case RequestType::Post:
      assert(false); // STILL TO DO
      // post(url, body);
      break;
    }
  
  d_buf_reqs.pop_front();
}

//----------------------------------------------------------------------//
void HttpOperations::processMessage()
{
  int messages = 0;
  CURLMsg* msg = curl_multi_info_read(d_curl_multi, &messages);

  assert(msg != nullptr);

  if (msg->msg != CURLMSG_DONE)
    {
      // LOG
      assert(false); // not sure why this would happen
      d_is_processing_req = false;
      return;
    }

  if (msg->data.result == CURLE_OPERATION_TIMEDOUT)
    {
      d_is_processing_req = false;
      d_owner->handleFailed(this,HttpOpError::Timeout);
      return;
    }

  // any other results
  if (msg->data.result != CURLE_OK)
    {
      // LOG THIS: the exact error details should be logged here
      // this will happen when failing to connect/initialize etc
      d_is_processing_req = false;
      d_owner->handleFailed(this,HttpOpError::Internal);
      return;
    }

  curl_easy_getinfo(d_curl,CURLINFO_RESPONSE_CODE,&d_resp_code);
  d_is_processing_req = false;
  curl_multi_remove_handle(d_curl_multi, d_curl);

  d_owner->handleResponse(this, d_resp_code, d_resp_headers, d_resp_body);
}

//----------------------------------------------------------------------//
size_t HttpOperations::respBodyWrite(char *data,
				     size_t size_mem,
				     size_t num_mem,
				     void *userdata)
{
  //std::cout << "HttpOperations::respBodyWrite" << std::endl;
  // userdata points to this
  // incrementally store the received body data
  HttpOperations* ptr = static_cast<HttpOperations*>(userdata);
  size_t num_bytes = size_mem*num_mem;
  ptr->d_resp_body.insert(ptr->d_resp_body.end(),
			  data,
			  data+num_bytes);
  return num_bytes;
}

//----------------------------------------------------------------------//
size_t HttpOperations::respHeaderWrite(char *data,
				       size_t size_mem,
				       size_t num_mem,
				       void *userdata)
{
  //std::cout << "HttpOperations::respHeaderWrite" << std::endl;
  // userdata points to this
  // store each header as it comes in
  size_t num_bytes = size_mem*num_mem;
  std::string header(data, num_bytes);
  HttpOperations* ptr = static_cast<HttpOperations*>(userdata);
  ptr->d_resp_headers.emplace_back(std::move(header));
  return num_bytes;
}

//----------------------------------------------------------------------//
int HttpOperations::timerRestart(CURLM *multi,
				 long timeout_ms,
				 void *userdata)
{
  //std::cout << "HttpOperations::timerRestart" << std::endl;
  //std::cout << "HttpOperations::timeout_ms = " << timeout_ms << std::endl;
  
  // userdata points to this
  HttpOperations* ptr = static_cast<HttpOperations*>(userdata);
  if (timeout_ms >= 0 && timeout_ms < 1000) // experienced receiving large timeouts
    {
      ptr->d_timer_process.restartMs(timeout_ms);
    }
  else if (timeout_ms < 0)
    {
      // do nothing (the timer disables when transfer is complete)
    }

  return 0;
}

//----------------------------------------------------------------------//
void HttpOperations::cancelBufferedReqs()
{
  //std::cout << "HttpOperations::cancelBufferedReqs()" << std::endl;
  d_buf_reqs.clear();
}
