#include "chttp_simple.h"

#include <cassert>

using namespace C_HTTP;

//----------------------------------------------------------------------//
SimpleHttp::SimpleHttp(SimpleHttpOwner* o)
  : d_owner(o)
{
  assert(o != nullptr);
  d_resp_headers.reserve(10*30); // assuming 10 headers each with 30 characters (assuming digest auth)
}

//----------------------------------------------------------------------//
SimpleHttp::~SimpleHttp()
{
  if (d_curl_multi != nullptr)
    {
      assert(d_curl != nullptr);
      curl_multi_remove_handle(d_curl_multi, d_curl);
      curl_easy_cleanup(d_curl);
      curl_multi_cleanup(d_curl_multi);
    }
  else
    {
      assert(d_curl == nullptr);
    }
	
  curl_global_cleanup();
}

//----------------------------------------------------------------------//
bool SimpleHttp::init(long timeout_sec)
{
  if (d_ready)
    {
      return true;
    }

  // multiple calls have the same affect as one call
  // NB: not thread safe
  if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) 
    {
      return false;
    }

  if (d_curl != nullptr)
    {
      curl_easy_cleanup(d_curl);
    }
  
  d_curl = curl_easy_init();
  if (d_curl == nullptr)
    {
      return false;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_TIMEOUT, timeout_sec) != CURLE_OK)
    {
      return false;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_WRITEFUNCTION, respBodyWrite) != CURLE_OK)
    {
      return false;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_WRITEDATA, this) != CURLE_OK)
    {
      return false;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_HEADERFUNCTION, respHeaderWrite) != CURLE_OK)
    {
      return false;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_HEADERDATA, this) != CURLE_OK)
    {
      return false;
    }

  d_curl_multi = curl_multi_init();
  if (d_curl_multi == nullptr)
    {
      return false;
    }
	
  if (curl_multi_setopt(d_curl_multi, CURLMOPT_TIMERFUNCTION, timerRestart) != CURLM_OK)
    {
      return false;
    }

  if (curl_multi_setopt(d_curl_multi, CURLMOPT_TIMERDATA, this) != CURLM_OK)
    {
      return false;
    }
	
  curl_multi_add_handle(d_curl_multi, d_curl);
  d_ready = true;
	
  return true;
}

//----------------------------------------------------------------------//
bool SimpleHttp::get(const std::string& url)
{
  if (d_running_transfers > 0)
    {
      // still transferring message
      assert(false);
      return false;
    }
	
  curl_easy_setopt(d_curl, CURLOPT_URL, url.c_str());
  // set the timer call back
  d_resp_body.clear();
  d_resp_headers.clear();
  d_timer_process.restartMs(0);
  
  return true;
}

//----------------------------------------------------------------------//
bool SimpleHttp::handleTimeOut(const KERN::KernelTimer& timer)
{
  if (d_timer_process.is(timer))
    {
      CURLMcode res_multi = curl_multi_perform(d_curl_multi, &d_running_transfers);

      if (res_multi != CURLM_OK)
	{
	  d_owner->handleFailed(SimpleError::Internal);
	  return true;
	}
			
      // has the transfer completed?
      if (d_running_transfers == 0)
	{
	  d_timer_process.disable();
	  processMessage();
	}
			
      return true;
    }
	
  return false;
}

//----------------------------------------------------------------------//
void SimpleHttp::processMessage()
{
  int messages = 0;
  CURLMsg* msg = curl_multi_info_read(d_curl_multi, &messages);

  assert(msg != nullptr);

  if (msg->msg != CURLMSG_DONE)
    {
      assert(false); // not sure why this would happen
      return;
    }

  if (msg->data.result == CURLE_OPERATION_TIMEDOUT)
    {
      d_owner->handleFailed(SimpleError::Timeout);
      return;
    }

  // any other results
  if (msg->data.result != CURLE_OK)
    {
      // LOG THIS: the exact error details should be logged here
      d_owner->handleFailed(SimpleError::Internal);
    }

  curl_easy_getinfo(d_curl,CURLINFO_RESPONSE_CODE,&d_resp_code);
  d_owner->handleResponse(d_resp_code, d_resp_headers, d_resp_body); 
}

//----------------------------------------------------------------------//
size_t SimpleHttp::respBodyWrite(char *data,
				 size_t size_mem,
				 size_t num_mem,
				 void *userdata)
{
  // userdata points to this
  // incrementally store the received body data
  SimpleHttp* ptr = static_cast<SimpleHttp*>(userdata);
  size_t num_bytes = size_mem*num_mem;
  ptr->d_resp_body.insert(ptr->d_resp_body.end(),
			  data,
			  data+num_bytes);
  return num_bytes;
}

//----------------------------------------------------------------------//
size_t SimpleHttp::respHeaderWrite(char *data,
				   size_t size_mem,
				   size_t num_mem,
				   void *userdata)
{
  // userdata points to this
  // store each header as it comes in
  size_t num_bytes = size_mem*num_mem;
  std::string header(data, num_bytes);
  SimpleHttp* ptr = static_cast<SimpleHttp*>(userdata);
  ptr->d_resp_headers.emplace_back(std::move(header));
  return num_bytes;
}

//----------------------------------------------------------------------//
int SimpleHttp::timerRestart(CURLM *multi,
			     long timeout_ms,
			     void *userdata)
{
  // userdata points to this
  SimpleHttp* ptr = static_cast<SimpleHttp*>(userdata);
  if (timeout_ms > 0)
    {
      ptr->d_timer_process.restartMs(timeout_ms);
    }
  else
    {
      ptr->d_timer_process.disable();
    }
  return 0;
}
