#ifndef DGP_SIMPLEGOPRO_H
#define DGP_SIMPLEGOPRO_H

#include "dgp_gopro.h"

#include <memory>

/*
  SimpleGoPro uses blocking type http commands (it is synchronous).
*/
namespace C_HTTP
{
  class SimpleHttpPost;
};

namespace D_GP
{
  class SimpleGoPro final : public GoPro
  {
  public:
    explicit SimpleGoPro(GoProOwner* o);

    void connectWithName(std::string) override;
    void setMode(Mode) override;
    void setShutter(bool) override;
  
  private:
    std::unique_ptr<C_HTTP::SimpleHttpPost> d_http_post;
    bool d_cmd_failed = false;
  };
};
#endif
