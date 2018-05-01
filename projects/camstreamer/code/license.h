#ifndef LICENSE_H
#define LICENSE_H

#include <string>

// using alternative name to increase difficulty of reverse engineering
// use in a timer with a value that is randomly generated within a range
// The validity of the user config file must be checked before using this class.
class Stream final
{
 public:
  explicit Stream();

  // This must be called before using any other non-static function.
  bool start();
  
 private:
  std::string getU(); // gets the user key from the license file
  std::string ob(const std::string& key); // obfuscates the key

  std::string getParam(const std::string& filename,
		       const std::string& match);
};

#endif
