#ifndef DGP_CONTROLLER_H
#define DGP_CONTROLLER_H

#include "dgp_gopro.h"
//#include "kn_timer.h" //implement this

#include <memory>
#include <vector>

namespace D_GP
{
	class GPState;
  class GoProController;
  class GoProControllerOwner
  {
  public:
    GoProControllerOwner() = default;
    virtual ~GoProControllerOwner() = default;
    GoProControllerOwner& operator=(const GoProControllerOwner&) = default;
    GoProControllerOwner(const GoProControllerOwner&) = default;
   
  private:
    friend GoProController;
		// the synchronous type controller will have undefined behaviour if it is not deleted after signaling a failure
    virtual void handleFailedRequest(GoProController*) = 0; 
  };

  class GoProController final : GoProOwner
  {
  public:
    struct GPCtrlParams
    {
      ControlType ctrl_type;
      std::string connect_name;
    };
    
  public:
    GoProController(GoProControllerOwner*, GPCtrlParams);

    void takePhoto();
    void StartStopRecording();

  private:
    //GoPro
    void handleModeSet(GoPro*, Mode) override {}
    void handleShutterSet(GoPro*, bool) override {}
    void handleFailedCommand(GoPro*, Cmd) override;

  private:
    void connectWithName(const std::string& name);
    void setState(StateId);
		StateId getPrevState() { return d_prev_state_id; }
    void processState();

	private:
		enum class StateId
		{
			Connect,
			Photo,
			StartStopRec
		};

		enum class Request
		{
			TakePhoto,
			StartStopRec
		};
		
  private:
		friend GPState;
		
    GoProControllerOwner* d_owner = nullptr;

    std::unique_ptr<GoPro> d_gp;
    bool d_is_recording = false;
    std::string d_connect_name;

		StateId d_state_id = StateId::Connect;
		StateId d_prev_state_id = StateId::Connect;
		std::vector<StateId, std::unique_ptr<GPState>> d_states;
		GPState* d_state = nullptr;
	};

	class GPState
	{
	public:
		virtual ~GPState() = default;
		virtual void process(GoProController&) = 0;
	};

	class StateConnect : GPState
	{
		void process(GoProController&) override;
	};
	
	class StatePhoto : GPState
	{
		void process(GoProController&) override;
	};

	class StateVideo : GPState
	{
		void process(GoProController&) override;
	};
};
#endif
