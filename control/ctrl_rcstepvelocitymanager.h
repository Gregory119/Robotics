#ifndef CTRL_RCSTEPVELOCITYMANAGER_H
#define CTRL_RCSTEPVELOCITYMANAGER_H

#include <memory>
#include <map>

namespace CORE
{
  class StepIncrementer;
};

namespace CTRL
{
  enum class VelocityStateId
    {
      Pos,
      Neg,
      Zero
    };
  
  class VelocityState;
  class ZeroVelocityState;
  class PosVelocityState;
  class NegVelocityState;
  class RCStepVelocityManager
  {
    // Useful for managing velocity requests from users for a Hobby Radio Control style ESC and motor. Changes the output velocity based on the current and requested output velocity over a time step.
  public:
    struct VelocityLimitParams
    {
      int abs_max_velocity = 0;
      int min_time_for_max_velocity_ms = 1;
      int time_step_ms = 0;
    };
    
  public:
    // keep units consistent
    RCStepVelocityManager(const VelocityLimitParams&);
    virtual ~RCStepVelocityManager();
    RCStepVelocityManager(const RCStepVelocityManager&) = delete;
    RCStepVelocityManager& operator=(const RCStepVelocityManager&) = delete;

    int stepToVelocity(int vel);
    // This must be called every time step
    // Input velocity sign implies direction.

    void setVelocity(int vel);
    int getVelocity();
    
  private:
    friend ZeroVelocityState;
    friend PosVelocityState;
    friend NegVelocityState;
    
    void decrementVelocity();
    void incrementVelocity();
    void setNextState(VelocityStateId);
    void updateState();

  private:
    std::map<VelocityStateId, std::unique_ptr<VelocityState>> d_states;
    VelocityState* d_state = nullptr;
    VelocityStateId d_next_state_id = VelocityStateId::Zero;
    
    std::unique_ptr<CORE::StepIncrementer> d_vel_inc; // holds the vel
  };

  //----------------------------------------------------------------------//
  class VelocityState
  {
  public:
    virtual ~VelocityState() = default;
    virtual void stepVelocity(RCStepVelocityManager&, int in_vel) = 0;
  };

  //----------------------------------------------------------------------//
  class ZeroVelocityState final : public VelocityState
  {
  public:
    void stepVelocity(RCStepVelocityManager&, int in_vel) override;
  };

  //----------------------------------------------------------------------//
  class PosVelocityState final : public VelocityState
  {
  public:
    void stepVelocity(RCStepVelocityManager&, int in_vel) override;    
  };

  //----------------------------------------------------------------------//
  class NegVelocityState final : public VelocityState
  {
  public:
    void stepVelocity(RCStepVelocityManager&, int in_vel) override;
  };
};

#endif
