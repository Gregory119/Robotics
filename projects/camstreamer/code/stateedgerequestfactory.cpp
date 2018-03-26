#include "stateedgerequestfactory.h"

//----------------------------------------------------------------------//
std::unique_ptr<StateEdgeRequest>
StateEdgeRequestFactory::create(const Params& params)
{
  switch (mode)
    {
    case ReqMode::Up:
    case ReqMode::Down:
    case ReqMode::Float:
      return std::unique_ptr<StateEdgeRequest>(new PinEdgeRequest(params.o,
								  params.req_num,
								  params.pull_mode));
      
    case ReqMode::Pwm:
      return std::unique_ptr<StateEdgeRequest>(new PwmEdgeRequest(params.o,
								  params.req_num,
								  params.pwm_limits_type));
      
    case ReqMode::Ppm:
      return std::unique_ptr<StateEdgeRequest>(new PpmEdgeRequest(params.o,
								  params.req_num,
								  state_ppmreader));
      
    case ReqMode::Unknown:
    }
  assert(false);
  return std::unique_ptr<StateEdgeRequest>();
}
