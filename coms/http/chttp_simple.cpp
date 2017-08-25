#include "chttp_simple.h"

#include <cassert>

using namespace C_HTTP;

//----------------------------------------------------------------------//
SimpleHttp::SimpleHttp(SimpleHttpOwner* o)
	: d_owner(o)
{
	assert(o != nullptr);
	d_resp_headers.reserve(6*20); // assuming 6 headers each with 20 characters (at least 3 headers)
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
  if (isReady())
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

	if (curl_easy_setopt(d_curl, CURLOPT_HEADER, ) != CURLE_OK)
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
	
	if (curl_easy_setopt(d_curl_multr, CURLOPT_TIMERFUNCTION, timerRestart) != CURLE_OK)
		{
			return false;
		}

	if (curl_easy_setopt(d_curl_multr, CURLOPT_TIMERDATA, this) != CURLE_OK)
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
		
	int transfers = 0;
  d_res_multi = curl_multi_perform(d_curl_multi, &transfers);
  if (d_res_multi != CURLE_OK)
    {
      return false;
    }
  return true;
}

//----------------------------------------------------------------------//
bool SimpleHttp::handleTimeOut(const KernelTimer& timer)
{
	if (d_timer.is(timer))
		{
			int transfers = 0;
			d_res_multi = curl_multi_perform(d_curl_multi, &transfers);

			if (d_res_multi != CURLE_OK)
				{
					d_owner->handleFailed(SimpleError::Internal);
					return true;
				}
			
			// has the transfer completed?
			if (transfers == 0)
				{
					d_timer.disable();
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
	d_owner->handleResponse(ResponseCode, d_resp_headers, d_resp_body); 
}

//----------------------------------------------------------------------//
static size_t SimpleHttp::respBodyWrite(char *ptr,
																				size_t size_mem,
																				size_t num_mem,
																				void *userdata)
{
	// userdata points to this
	// incrementally store the received body data
	userdata->d_resp_body.insert(userdata->d_resp_body.end(),
															 ptr,
															 ptr+size_mem*num_mem);
}

//----------------------------------------------------------------------//
static size_t SimpleHttp::respHeaderWrite(char *ptr,
																					size_t size_mem,
																					size_t num_mem,
																					void *userdata)
{
	// userdata points to this
	// store each header as it comes in
	std::string header(ptr, size_mem*num_mem);
	userdata->d_resp_headers.emplace_back(std::move(header));
}

//----------------------------------------------------------------------//
static int SimpleHttp::timerCallback(CURLM *multi,
																		 long timeout_ms,
																		 void *userdata)
{
	// userdata points to this
	userdata->d_timer.restartMs(timeout_ms);
}
