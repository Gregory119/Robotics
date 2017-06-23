#ifndef TIMERSERVER_H
#define TIMERSERVER_H

#include <QObject>

class TimerServer final : QObject
{
  Q_OBJECT

 public:
  TimerServer();
  
 private slots:
   void slotTestTimeOut();
  
 private:
  QTimer* d_test_timer;
  
  int d_value = 0;
};

#endif
