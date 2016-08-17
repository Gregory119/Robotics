#include <wiringSerial.h>
#include <iostream>
#include <unistd.h> //for sleep

int main()
{
  int baud = 9600;
  std::cout<<"opening the serial port now"<<std::endl;
  int ser_desc = serialOpen("/dev/ttyAMA0", baud);
  //pi => /dev/ttyAMA0

  if (ser_desc == -1)
    {
     return -1;
    }

  for (unsigned i=0; i<5;++i)
    {
      serialPutchar(ser_desc, 'G');
      sleep(1); //in seconds
    }      

  std::cout<<"closing the serial port now"<<std::endl;
      
  serialClose(ser_desc);
}
