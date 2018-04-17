#include "ppmedgerequest.h"
#include "crc_ppmreader.h"

//----------------------------------------------------------------------//
PpmEdgeRequest::PpmEdgeRequest(StateEdgeRequest::Owner* o,
			       int num,
			       C_RC::StatePpmReader* reader)
  : StateEdgeRequest(o),
    d_channel(static_cast<unsigned>(num)),
    d_reader(reader)
{
  if (d_channel > C_RC::getMaxPpmChannels() ||
      d_channel == 0)
    {
      std::ostringstream stream("PpmEdgeRequest::PpmEdgeRequest - The request number of '",
				std::ios_base::app);
      stream << num << "' is not a valid channel number. It must be less than or equal to "
	     << C_RC::getMaxPpmChannels() << " and positive.";
      d_err_msg = stream.str();
      d_error_timer.singleShotZero([&](){
	  ownerError(d_err_msg);
	});
      return;
    }
  
  d_reader->attachObserver(this);
}

//----------------------------------------------------------------------//
void PpmEdgeRequest::start()
{
  if (d_reader == nullptr)
    {
      assert(false);
      return;
    }
  // multiple instances of this class, who are observers, will call this function
  d_reader->start();
}

//----------------------------------------------------------------------//
void PpmEdgeRequest::stop()
{
  if (d_reader == nullptr)
    {
      assert(false);
      return;
    }
  // multiple instances of this class, who are observers, will call this function
  d_reader->stop();
}

//----------------------------------------------------------------------//
// StatePpmReader::Observer
//----------------------------------------------------------------------//
void PpmEdgeRequest::handleState(C_RC::PpmReader<unsigned>*, bool state)
{
  processState(state);
}

//----------------------------------------------------------------------//
void PpmEdgeRequest::handleError(C_RC::PpmReader<unsigned>*,
				 C_RC::PpmReaderError,
				 const std::string& msg)
{
  std::string new_msg = "PpmEdgeRequest::handleError - Failed with the following error:\n";
  new_msg += msg;
  ownerError(new_msg);
}
