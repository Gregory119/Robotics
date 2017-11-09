#include <iostream>
//#include <chrono>
//#include <thread>

class Test
{
public:
  Test() = default;
  Test(const Test&) = delete;
  Test& operator=(const Test&) = delete;
  //Test(const Test&&) = delete;
  //Test& operator=(const Test&&) = delete;

private:
  std::string d_str = "twenty";
};

int main(int argc, char** argv)
{
  Test test1;
  Test test2;

  test1 = std::move(test2);
  
  /* while (1)
    {
      //std::this_thread::sleep_for(std::chrono::seconds(1));
      }*/
  
  return 0;
}
