#ifndef CORE_FILEPARAMMANAGER_H
#define CORE_FILEPARAMMANAGER_H

#include <fstream>
#include <string>

namespace CORE
{
  // A general assumption is that only one parameter and its associated value reside on each line.
  class FileParamManager
  {
  public:
    enum class Error
    {
      None,
	FileOpen,
	ParamNotFound,
	MultiParamDef,
	FileCharacterExtraction,
	ParamExtraction,
	SetFilePosIndicator,
	OutputToFile,
	SetParamException,
	GetParamException,
	GetParamValuePosException
    };
    
  public:
    // no errors will occur on construction
    FileParamManager(std::string file_path);

    void enableFailOnDuplicateParams();
    
    // eg. pin_number=5, which implies name_match is "pin_number=" and the returned string is "5".
    // The file is first opened, then the first occurence of name_match is searched for, and finally the file is closed when returning the found parameter string. It is assumed that there is one parameter per line; the returned value will contain all the line text after the name_match.
    // On failure the returned string is empty. 
    std::string getParam(const std::string& name_match);

    void setParamFromTo(const std::string& name_match,
			const std::string& new_value);

    const std::string& getFilePath() const { return d_file_path; }
    std::string str(); // of the entire file contents
    
    // Removes the start and end quotes of a parameter string. Only double style quotes can be used.
    // Eg. "five" changes to five. On error the no_quotes parameter is not modified.
    // On success the no_quotes parameter is cleared and set.
    static bool removeQuotesFromTo(const std::string& orig, std::string& no_quotes);

    // All error related functions reflect all the errors that occured within the non-static functions
    bool hasError() const { return d_error != Error::None; }
    Error getError() const { return d_error; }

    // Error messages include the file path, but do not include a new line character at the end.
    std::string& getErrorMsg() { return d_error_msg; }

  private:
    // Returns pos_type(-1) on failure.
    std::iostream::pos_type getParamValuePos(std::fstream& file,
					     const std::string& name_match);

    // Sets the input position indicator.
    void goToParamValue(std::fstream& file,
			const std::string& name_match);
    
    void setError(Error, const std::string& msg);
    
  private:
    const std::string d_file_path;

    Error d_error = Error::None;
    std::string d_error_msg;

    bool d_check_duplicate_params = false;
  };
};

#endif
