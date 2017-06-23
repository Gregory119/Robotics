#include "timerserver.h"

#include <iostream>

//----------------------------------------------------------------------//
TimerServer::TimerServer()
{
  d_test_timer = new QTimer(this);
  connect(d_test_timer,SIGNAL(timeout()),this,SLOT(slotTestTimeOut()));
  d_test_timer->start(1000);
}

//----------------------------------------------------------------------//
void TimerServer::slotTestTimeOut()
{
  std::cout << "d_value = " << ++d_value << std::endl;

  if (d_value == 10)
    {
      d_test_timer->stop();
    }
}
