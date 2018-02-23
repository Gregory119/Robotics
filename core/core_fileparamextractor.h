#ifndef CORE_FILEPARAMEXTRACTOR_H
#define CORE_FILEPARAMEXTRACTOR_H

#include <string>

namespace CORE
{
  class FileParamExtractor
  {
  public:
    enum class Error
    {
      None,
	FileOpen,
	ParamNotFound,
	FileCharacterExtraction,
	ParamExtraction
    };
    
  public:
    FileParamExtractor(std::string file_path);

    // eg. pin_number=5, which implies name_match is "pin_number=" and the returned string is "5".
    // The file is first opened, then the first occurence of name_match is searched for, and finally the file is closed when returning the found parameter string. It is assumed that there is one parameter per line; the returned value will contain all the line text after the name_match.
    // On failure the returned string is empty.
    std::string getParam(const std::string& name_match);

    template <class T>
      void setParam(const std::string& name_match, const T&)
      {
	// find match in file
	// copy file contents up to the end of the match to a temporary output file
	// output the new parameter value to the temporary file
	// copy the rest of the original file, excluding the parameter value, into the temporary file
	// delete the original file and rename the temporary file to the original file name
      }
    
    bool hasError() { return d_error != Error::None; }
    Error getError() { return d_error; }
    std::string& getErrorMsg() { return d_error_msg; }
    const std::string& getFilePath() { return d_file_path; }

  private:
    void setError(Error, const std::string& msg);
    
  private:
    const std::string d_file_path;

    Error d_error = Error::None;
    std::string d_error_msg;
  };
};

#endif
