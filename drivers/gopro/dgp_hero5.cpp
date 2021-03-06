#include "dgp_hero5.h"

#include "dgp_utils.h"

#include <cassert>
#include <chrono>

using namespace D_GP;

//----------------------------------------------------------------------//
GoProHero5::GoProHero5(GoPro::Owner* o, const std::string& name)
  : GoPro(o),
    d_http(new C_HTTP::Operations(this, std::chrono::seconds(10))),
    d_udp_client(new C_UDP::Client(this,
				   "10.5.5.9",//D_GP::Utils::ipAddr(CamModel::Hero5),
				   "8554"))
{
  assert(o != nullptr);
  assert(!name.empty());
  d_connect_name = name;

  d_timer_stream.setTimeoutCallback([this](){
      // do not request to maintain stream if already waiting on a maintain stream response
      /*if (!d_cmd_reqs.empty())
	      
	{
	  auto it = std::find(d_cmd_reqs.begin(),
			      d_cmd_reqs.end(),
			      GoPro::Cmd::MaintainStream);
	  if (it != d_cmd_reqs.end())
	    {
	      // still waiting on the response to maintain the live stream
	      return;
	    }
	    }*/
      maintainStream();
    });

  d_http->appendHeaders({"Connection: Keep-Alive"});
}

//----------------------------------------------------------------------//
void GoProHero5::connect()
{
  // LOG
  std::vector<std::string> params = {d_connect_name};
  d_http->get(Utils::cmdToUrl(GoPro::Cmd::Connect,
			      CamModel::Hero5,
			      params));
  d_cmd_reqs.push_back(GoPro::Cmd::Connect);
}

//----------------------------------------------------------------------//
void GoProHero5::status()
{
  requestCmd(GoPro::Cmd::Status);
}

//----------------------------------------------------------------------//
void GoProHero5::setMode(Mode mode)
{
  // LOG
  switch (mode)
    {
    case Mode::VideoNormal:
      requestCmd(GoPro::Cmd::SetModeVideoNormal);
      return;
      
    case Mode::VideoTimeLapse:
      requestCmd(GoPro::Cmd::SetModeVideoTimeLapse);
      return;
      
    case Mode::VideoPlusPhoto:
      requestCmd(GoPro::Cmd::SetModeVideoPlusPhoto);
      return;
      
    case Mode::VideoLooping:
      requestCmd(GoPro::Cmd::SetModeVideoLooping);
      return;
      
    case Mode::PhotoSingle:
      requestCmd(GoPro::Cmd::SetModePhotoSingle);
      return;
      
    case Mode::PhotoContinuous:
      requestCmd(GoPro::Cmd::SetModePhotoContinuous);
      return;
      
    case Mode::PhotoNight:
      requestCmd(GoPro::Cmd::SetModePhotoNight);
      return;
      
    case Mode::MultiShotBurst:
      requestCmd(GoPro::Cmd::SetModeMultiShotBurst);
      return;
      
    case Mode::MultiTimeLapse:
      requestCmd(GoPro::Cmd::SetModeMultiShotTimeLapse);
      return;
      
    case Mode::MultiNightLapse:
      requestCmd(GoPro::Cmd::SetModeMultiShotNightLapse);
      return;
	
    case Mode::Unknown:
      assert(false);
      ownerCmdFailed(GoPro::Cmd::Unknown,
		     GoPro::Error::Internal,
		     "setMode - Failed. The mode is unknown.");
      return;
    }
  assert(false);
  std::ostringstream stream("setMode - Failed. The mode value of ",
			    std::ios_base::app);
  stream << static_cast<int>(mode) << " is invalid.";
  ownerCmdFailed(GoPro::Cmd::Unknown,
		 GoPro::Error::Internal,
		 stream.str());
}

//----------------------------------------------------------------------//
void GoProHero5::setShutter(bool state)
{
  // LOG
  GoPro::Cmd cmd = GoPro::Cmd::SetShutterStop;
  if (state)
    {
      cmd = GoPro::Cmd::SetShutterTrigger;
    }
  // else request to stop shutter
  d_http->get(Utils::cmdToUrl(cmd,
			      CamModel::Hero5));
  d_cmd_reqs.push_back(cmd);
}

//----------------------------------------------------------------------//
void GoProHero5::setBitRatePerSecond(unsigned bitrate)
{
  std::vector<std::string> params = {std::to_string(bitrate)};
  GoPro::Cmd cmd = GoPro::Cmd::SetBitRate;
  d_http->get(Utils::cmdToUrl(cmd,
			      CamModel::Hero5,
			      params));
  d_cmd_reqs.push_back(cmd);
}

//----------------------------------------------------------------------//
void GoProHero5::startLiveStream()
{
  requestCmd(GoPro::Cmd::StartLiveStream);
  if (!d_is_streaming)
    {
      d_udp_client->connect(std::chrono::seconds(3));
    }
}

//----------------------------------------------------------------------//
void GoProHero5::stopLiveStream()
{
  internalStopLiveStream();
  ownerCommandSuccessful(GoPro::Cmd::StopLiveStream);
}

//----------------------------------------------------------------------//
bool GoProHero5::hasBufferedCmds()
{
  return d_http->hasBufferedReqs();
}

//----------------------------------------------------------------------//
void GoProHero5::cancelBufferedCmds()
{
  if (!d_http->hasBufferedReqs())
    {
      // Why not to clear the reqs list at this point:
      // If a request is being processed here, and there are no buffers,
      // then the request being processed is the front command request.
      // This request must not be cleared because it is needed for the handle call backs.
#ifndef RELEASE
      if (d_http->isProcessingReq())
	{
	  assert(d_cmd_reqs.size() == 1);
	}
      else
	{
	  assert(d_cmd_reqs.empty());
	}
#endif
      return;
    }

  if (d_http->isProcessingReq())
    {
      assert(d_cmd_reqs.size() > 1); // one processing and at least one buffered
      d_cmd_reqs.erase(++d_cmd_reqs.begin(),d_cmd_reqs.end());
    }
  
  d_http->cancelBufferedReqs();
}

//----------------------------------------------------------------------//
void GoProHero5::handleResponse(C_HTTP::Operations* http,
				C_HTTP::ResponseCodeNum code,
				const std::vector<std::string>& headers,
				const std::vector<char>& body)
{
  assert(!d_cmd_reqs.empty());
  GoPro::Cmd cmd = d_cmd_reqs.front();
  d_cmd_reqs.pop_front();
  
  if (code >= static_cast<C_HTTP::ResponseCodeNum>(C_HTTP::ResponseCode::BadRequest))
    {
      // not successful
      std::ostringstream stream("handleResponse - Received an error response code with the value ",
				std::ios_base::app);
      stream << static_cast<int>(code) << ".";
      ownerCmdFailed(cmd,
		     GoPro::Error::Response,
		     stream.str());
      return;
    }

  switch (cmd)
    {
    case GoPro::Cmd::Connect:
    case GoPro::Cmd::SetModePhotoSingle:
    case GoPro::Cmd::SetModePhotoContinuous:
    case GoPro::Cmd::SetModePhotoNight:
    case GoPro::Cmd::SetModeVideoNormal:
    case GoPro::Cmd::SetModeVideoTimeLapse:
    case GoPro::Cmd::SetModeVideoPlusPhoto:
    case GoPro::Cmd::SetModeVideoLooping:
    case GoPro::Cmd::SetModeMultiShotBurst:
    case GoPro::Cmd::SetModeMultiShotTimeLapse:
    case GoPro::Cmd::SetModeMultiShotNightLapse:
    case GoPro::Cmd::SetShutterTrigger:
    case GoPro::Cmd::SetShutterStop:
    case GoPro::Cmd::SetBitRate:
      ownerCommandSuccessful(cmd);
      return;

    case GoPro::Cmd::StartLiveStream:
      d_is_streaming = true;
      ownerCommandSuccessful(cmd);
      return;
      
    case GoPro::Cmd::Status:
      {
	if (body.empty())
	  {
	    ownerCmdFailed(cmd,
			   GoPro::Error::Response,
			   "handleResponse - Received an empty body response to a GoPro Status query.");
	    return;
	  }
	
	std::string body_str = std::string(body.begin(),body.end());
	if (d_status.loadStr(body_str, CamModel::Hero5))
	  {
	    //d_status.print();
	    ownerCommandSuccessful(cmd);
	    return;
	  }
	// LOG
	std::ostringstream stream("handleResponse - Failed to extract the Status data from the following response body:\n",
				  std::ios_base::app);
	stream << body_str;
	ownerCmdFailed(cmd,
		       GoPro::Error::ResponseData,
		       stream.str());
      }
      return;

    case GoPro::Cmd::StopLiveStream: // should not be handled here
    case GoPro::Cmd::Unknown:
      assert(false);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
void GoProHero5::handleFailed(C_HTTP::Operations* http,
			      C_HTTP::OpError error,
			      const std::string& msg)
{
  // sent command was unsuccessful
  GoPro::Cmd cmd = d_cmd_reqs.front();
  d_cmd_reqs.pop_front();

  if (cmd == GoPro::Cmd::StartLiveStream)
    {
      internalStopLiveStream();
    }

  std::string new_msg = "handleFailed(C_HTTP::Operations*...) - ";
  
  switch (error)
    {
    case C_HTTP::OpError::Internal:
      new_msg += "Internal failure with the following message:\n";
      new_msg += msg;
      ownerCmdFailed(cmd,
		     GoPro::Error::Internal,
		     new_msg);
      return;

    case C_HTTP::OpError::Timeout:
      new_msg += "Timeout failure with the following message:\n";
      new_msg += msg;
      ownerCmdFailed(cmd,
		     GoPro::Error::Timeout,
		     new_msg);
      return;

    case C_HTTP::OpError::Response:
      new_msg += "Response failure with the following message:\n";
      new_msg += msg;
      ownerCmdFailed(cmd,
		     GoPro::Error::Response,
		     new_msg);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
void GoProHero5::handleConnected(C_UDP::Client*)
{
  d_timer_stream.restartIfNotSetOrDisabled(std::chrono::milliseconds(2500));
}

//----------------------------------------------------------------------//
void GoProHero5::handleMessageSent(C_UDP::Client*)
{
  // timer will send the next one
}

//----------------------------------------------------------------------//
void GoProHero5::handleFailed(C_UDP::Client*, C_UDP::Client::Error err)
{
  switch (err)
    {
    case C_UDP::Client::Error::Connect:
      // LOG
      // Wifi probably not active. Allowing the udp reconnect timer to reconnect.
      return;

    case C_UDP::Client::Error::AlreadySending:
      // LOG
      assert(false);
      return;

    case C_UDP::Client::Error::Disconnected:
      // LOG
      internalStopLiveStream();
      ownerStreamDown();
      return;
      
    case C_UDP::Client::Error::Unknown:
      // LOG
      assert(false);
      return;
    }
}

//----------------------------------------------------------------------//
void GoProHero5::internalStopLiveStream()
{
  d_timer_stream.disable();
  d_is_streaming = false;
}

//----------------------------------------------------------------------//
void GoProHero5::maintainStream()
{
  // send udp message
  if (d_udp_client->isSending())
    {
      return; // wait for send to complete before sending again
    }
  std::string str = "_GPHD_:0:0:2:" + std::to_string(2.0) + "\n";
  d_udp_client->send(str.data(), str.size());
}

//----------------------------------------------------------------------//
void GoProHero5::requestCmd(GoPro::Cmd cmd)
{
  d_http->get(Utils::cmdToUrl(cmd,
			      CamModel::Hero5));
  d_cmd_reqs.push_back(cmd);
}

//----------------------------------------------------------------------//
void GoProHero5::ownerCmdFailed(GoPro::Cmd cmd,
				GoPro::Error e,
				const std::string& msg)
{
  std::string new_msg = "D_GP::GoProHero5::";
  new_msg += msg;
  ownerCommandFailed(cmd, e, new_msg);
}
