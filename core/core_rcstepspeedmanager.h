#ifndef CORE_RCSTEPSPEEDMANAGER_H
#define CORE_RCSTEPSPEEDMANAGER_H

#include <memory>
#include <map>

namespace CORE
{
  enum class SpeedStateId
    {
      Pos,
      Neg,
      Zero
    };
  
  class StepIncrementer;
  class SpeedState;
  class ZeroSpeedState;
  class PosSpeedState;
  class NegSpeedState;
  class RCStepSpeedManager
  {
    // Useful for managing speed requests from users for a Hobby Radio Control style ESC and motor. Changes the output speed based on the current and requested output speed over a time step.
  public:
    struct SpeedLimitParams
    {
      int abs_max_speed = 0;
      int min_time_for_max_speed_ms = 1;
      int time_step_ms = 0;
    };
    
  public:
    // keep units consistent
    RCStepSpeedManager(const SpeedLimitParams&);
    virtual ~RCStepSpeedManager() = default;
    RCStepSpeedManager(const RCStepSpeedManager&) = delete;
    RCStepSpeedManager& operator=(const RCStepSpeedManager&) = delete;

    int stepSpeed(int input_speed);
    // This must be called every time step
    // Input speed sign implies direction.

    int getSpeed() { return d_speed; }
    
  private:
    friend ZeroSpeedState;
    friend PosSpeedState;
    friend NegSpeedState;
    void decrementSpeed();
    void incrementSpeed();
    void setSpeed(int in_speed) { d_speed = in_speed; }
    void setNextState(SpeedStateId);
    void updateState();

  private:
    std::map<SpeedStateId, std::unique_ptr<SpeedState>> d_states;
    SpeedState* d_state = nullptr;
    SpeedStateId d_next_state_id = SpeedStateId::Zero;
    
    std::unique_ptr<StepIncrementer> d_step_inc;
    int d_speed = 0;
  };

  //----------------------------------------------------------------------//
  class SpeedState
  {
  public:
    virtual ~SpeedState() = default;
    virtual void stepSpeed(RCStepSpeedManager&, int input_speed) = 0;
  };

  //----------------------------------------------------------------------//
  class ZeroSpeedState final : public SpeedState
  {
  public:
    void stepSpeed(RCStepSpeedManager&, int input_speed) override;
  };

  //----------------------------------------------------------------------//
  class PosSpeedState final : public SpeedState
  {
  public:
    void stepSpeed(RCStepSpeedManager&, int input_speed) override;    
  };

  //----------------------------------------------------------------------//
  class NegSpeedState final : public SpeedState
  {
  public:
    void stepSpeed(RCStepSpeedManager&, int input_speed) override;
  };
};

#endif
