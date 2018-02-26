#ifndef CORE_FILEPARAMMANIPULATOR_H
#define CORE_FILEPARAMMANIPULATOR_H

#include <istream>
#include <string>

namespace CORE
{
  // A general assumption is that only one parameter and its associated value reside on each line.
  class FileParamManipulator
  {
  public:
    enum class Error
    {
      None,
	FileOpen,
	ParamNotFound,
	FileCharacterExtraction,
	ParamExtraction,
	ParamUnchanged
    };
    
  public:
    FileParamManipulator(std::string file_path);

    // eg. pin_number=5, which implies name_match is "pin_number=" and the returned string is "5".
    // The file is first opened, then the first occurence of name_match is searched for, and finally the file is closed when returning the found parameter string. It is assumed that there is one parameter per line; the returned value will contain all the line text after the name_match.
    // On failure the returned string is empty. The file input and output position indicators are reset to zero before a successful return.
    std::string getParam(const std::string& name_match);

    bool setParam(const std::string& name_match,
		  const std::string& new_value);
    
    bool hasError() { return d_error != Error::None; }
    Error getError() { return d_error; }

    // Messages contain an new line character at the end.
    std::string& getErrorMsg() { return d_error_msg; }
    const std::string& getFilePath() { return d_file_path; }

  private:
    // Resets the position indicators before a successful return. Returns pos_type(-1) on failure.
    std::iostream::pos_type getParamValuePos(const std::string& name_match);

    // Sets the input position indicator
    void goToParamValue(const std::string& name_match);
    
    void setError(Error, const std::string& msg);
    
  private:
    const std::string d_file_path;
    std::fstream d_file;

    Error d_error = Error::None;
    std::string d_error_msg;
  };
};

#endif
