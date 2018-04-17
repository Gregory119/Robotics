#include "chttp_operations.h"

#include <cassert>
#include <chrono>

using namespace C_HTTP;

//----------------------------------------------------------------------//
Operations::Operations(Owner* o,
		       std::chrono::seconds timeout)
  : d_owner(o),
    d_timeout(timeout)
{
  assert(o != nullptr);
  d_resp_headers.reserve(10*30); // assuming 10 headers each with 30 characters (assuming digest auth)

  d_timer_process.setTimeoutCallback([this](){
      process();
    });

  removeDefaultHeaders({"Accept:"});

  d_timer_error.setTimeoutCallback([this](){
      ownerFailed(OpError::Internal,
		  "Operations - Failed to initialize.");
    });

  if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) 
    {
      d_timer_error.singleShotZero();
      return;
    }

  d_curl = curl_easy_init();
  if (d_curl == nullptr)
    {
      d_timer_error.singleShotZero();
      return;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_TIMEOUT, d_timeout.count()) != CURLE_OK)
    {
      d_timer_error.singleShotZero();
      return;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_WRITEFUNCTION, respBodyWrite) != CURLE_OK)
    {
      d_timer_error.singleShotZero();
      return;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_WRITEDATA, this) != CURLE_OK)
    {
      d_timer_error.singleShotZero();
      return;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_HEADERFUNCTION, respHeaderWrite) != CURLE_OK)
    {
      d_timer_error.singleShotZero();
      return;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_HEADERDATA, this) != CURLE_OK)
    {
      d_timer_error.singleShotZero();
      return;
    }

  if (d_header_list != nullptr)
    {
      if (curl_easy_setopt(d_curl, CURLOPT_HTTPHEADER, d_header_list) != CURLE_OK)
	{
	  d_timer_error.singleShotZero();
	  return;
	}
    }

  // debug info
  /*
#ifndef RELEASE
  if (curl_easy_setopt(d_curl, CURLOPT_VERBOSE, 1L) != CURLE_OK)
    {
      d_timer_error.singleShotZero();
      return;
    }
#endif
  */
  d_curl_multi = curl_multi_init();
  if (d_curl_multi == nullptr)
    {
      d_timer_error.singleShotZero();
      return;
    }
	
  if (curl_multi_setopt(d_curl_multi, CURLMOPT_TIMERFUNCTION, timerRestart) != CURLM_OK)
    {
      d_timer_error.singleShotZero();
      return;
    }

  if (curl_multi_setopt(d_curl_multi, CURLMOPT_TIMERDATA, this) != CURLM_OK)
    {
      d_timer_error.singleShotZero();
      return;
    }

  d_is_initialised = true;
}

//----------------------------------------------------------------------//
Operations::~Operations()
{
  if ( d_curl != nullptr)
    {
      curl_easy_cleanup(d_curl);
    }

  if (d_curl_multi != nullptr)
    {
      curl_multi_cleanup(d_curl_multi);
    }

  if (d_header_list != nullptr)
    {
      curl_slist_free_all(d_header_list);
    }
  
  curl_global_cleanup();
}

//----------------------------------------------------------------------//
void Operations::appendHeaders(const std::list<std::string>& headers)
{
  for (const auto& header : headers)
    {
      d_header_list = curl_slist_append(d_header_list, header.c_str());
    }
}

//----------------------------------------------------------------------//
// eg. "Accept:"
void Operations::removeDefaultHeaders(const std::list<std::string>& headers) 
{
  appendHeaders(headers);
}

//----------------------------------------------------------------------//
void Operations::get(const std::string& url)
{
  if (!d_is_initialised)
    {
      assert(false);
      return;
    }
	
  if (d_is_processing_req)
    {
      Request req;
      req.type = RequestType::Get;
      req.url = url;
      d_buf_reqs.push_back(req);
      return;
    }

  d_is_processing_req = true;
  d_url = url;  
  curl_easy_setopt(d_curl, CURLOPT_URL, url.c_str());
  curl_multi_add_handle(d_curl_multi, d_curl);
  
  d_resp_body.clear();
  d_resp_headers.clear();
  d_timer_process.restart(std::chrono::milliseconds(0));
}

//----------------------------------------------------------------------//
void Operations::process()
{
  CURLMcode res_multi = curl_multi_perform(d_curl_multi, &d_running_transfers);

  if (res_multi != CURLM_OK)
    {
      // LOG THIS: the exact error details should be logged here
      assert(false); // this should not happen
      d_timer_process.disable();
      curl_multi_remove_handle(d_curl_multi, d_curl);
      std::string msg = "process - Failed to perform a transfer with the following error:\n";
      msg += std::string(curl_multi_strerror(res_multi));
      ownerFailed(OpError::Internal, msg);
      return;
    }
			
  // has the transfer completed?
  if (d_running_transfers == 0)
    {
      d_timer_process.disable();
      processMessage();
      processNextBufferedReq(); // next request can start after owner messages (owner may want to clear/stop the buffered messages)
    }		
}

//----------------------------------------------------------------------//
void Operations::processNextBufferedReq()
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
void Operations::processMessage()
{
  int messages = 0;
  CURLMsg* msg = curl_multi_info_read(d_curl_multi, &messages);
  assert(msg != nullptr);

  while (msg != nullptr)
    {
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
	  curl_multi_remove_handle(d_curl_multi, d_curl);
	  std::ostringstream stream("processMessage - The operation timed out after ",
				    std::ios_base::app);
	  stream << d_timeout.count() << " seconds.";
	  ownerFailed(OpError::Timeout, stream.str());
	  return;
	}

      // any other results
      if (msg->data.result != CURLE_OK)
	{
	  // LOG THIS: the exact error details should be logged here
	  // this will happen when failing to connect/initialize etc
	  d_is_processing_req = false;
	  curl_multi_remove_handle(d_curl_multi, d_curl);
	  std::string message = "processMessage - The response message failed with the following error:\n";
	  message += std::string(curl_easy_strerror(msg->data.result));
	  ownerFailed(OpError::Response, message);
	  return;
	}

      curl_easy_getinfo(d_curl, CURLINFO_RESPONSE_CODE, &d_resp_code);
      d_is_processing_req = false;
      curl_multi_remove_handle(d_curl_multi, d_curl);

      d_owner.call(&Owner::handleResponse, this, d_resp_code, d_resp_headers, d_resp_body);

      // get the next message
      msg = curl_multi_info_read(d_curl_multi, &messages);
    }
}

//----------------------------------------------------------------------//
size_t Operations::respBodyWrite(char *data,
				 size_t size_mem,
				 size_t num_mem,
				 void *userdata)
{
  // userdata points to this
  // incrementally store the received body data
  Operations* ptr = static_cast<Operations*>(userdata);
  size_t num_bytes = size_mem*num_mem;
  ptr->d_resp_body.insert(ptr->d_resp_body.end(),
			  data,
			  data+num_bytes);
  return num_bytes;
}

//----------------------------------------------------------------------//
size_t Operations::respHeaderWrite(char *data,
				   size_t size_mem,
				   size_t num_mem,
				   void *userdata)
{
  // userdata points to this
  // store each header as it comes in
  size_t num_bytes = size_mem*num_mem;
  std::string header(data, num_bytes);
  Operations* ptr = static_cast<Operations*>(userdata);
  ptr->d_resp_headers.emplace_back(std::move(header));
  return num_bytes;
}

//----------------------------------------------------------------------//
int Operations::timerRestart(CURLM *multi,
			     long timeout_ms,
			     void *userdata)
{
  // userdata points to this
  Operations* ptr = static_cast<Operations*>(userdata);
  if (timeout_ms >= 0 && timeout_ms < 1000) // experienced receiving large timeouts
    {
      ptr->d_timer_process.restart(std::chrono::milliseconds(timeout_ms));
    }
  else if (timeout_ms < 0)
    {
      // do nothing (the timer disables when transfer is complete)
    }

  return 0;
}

//----------------------------------------------------------------------//
void Operations::cancelBufferedReqs()
{
  d_buf_reqs.clear();
}

//----------------------------------------------------------------------//
void Operations::ownerFailed(OpError e, const std::string& msg)
{
  std::string new_msg = "C_HTTP::Operations::";
  new_msg += msg;
  d_owner.call(&Owner::handleFailed, this, e, new_msg);
}
