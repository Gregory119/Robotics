#include "core_fileparammanager.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <sstream>

using namespace CORE;

//----------------------------------------------------------------------//
FileParamManager::FileParamManager(std::string file_path)
  : d_file_path(std::move(file_path))
{}

//----------------------------------------------------------------------//
std::string FileParamManager::getParam(const std::string& name_match)
{
  d_error = Error::None;
  d_error_msg.clear();
  
  std::fstream file(d_file_path);
  if (file.fail())
    {
      std::ostringstream stream("FileParamManager - Failed to open the file '",
				std::ios_base::app);
      stream << d_file_path << "'.";
      setError(Error::FileOpen, stream.str());
      return std::string();
    }
  
  std::string value;
  try
    {
      goToParamValue(file,name_match);
      if (hasError())
	{
	  return std::string();
	}
      
      file >> value;

      if (file.fail())
	{
	  std::ostringstream stream("getParam - Failed to extract the parameter value of the parameter '",
				    std::ios_base::app);
	  stream << name_match << "' from the file '" << d_file_path << "'.";
	  setError(Error::ParamExtraction, stream.str());
	  return std::string();
	}
    }
  catch (...)
    {
      std::ostringstream stream("getParam - Failed to extract the parameter value of the parameter '",
				std::ios_base::app);
      stream << name_match << "' from the file '" << d_file_path << "'.";
      setError(Error::GetParamException, stream.str());
      return std::string();
    }
  return value;
}

//----------------------------------------------------------------------//
void FileParamManager::setParamFromTo(const std::string& name_match,
				      const std::string& new_value)
{
  d_error = Error::None;
  d_error_msg.clear();

  std::fstream file(d_file_path);
  if (file.fail())
    {
      std::ostringstream stream("FileParamManager - Failed to open the file '",
				std::ios_base::app);
      stream << d_file_path << "'.";
      setError(Error::FileOpen, stream.str());
      return;
    }
  
  try
    {
      // search and get the parameter value position and value
      std::iostream::pos_type val_pos = getParamValuePos(file, name_match);
      if (file.fail())
	{
	  std::ostringstream stream("setParam - Failed to set the parameter value because of the following error: \n",
				    std::ios_base::app);
	  stream << getErrorMsg();
	  setError(Error::SetFilePosIndicator, stream.str());
	  return;
	}
      
      file.seekg(val_pos); //set input pos indicator
      if (file.fail())
	{
	  std::ostringstream stream("setParam - Failed to set the input position indicator to ",
				    std::ios_base::app);
	  stream << val_pos << "' in the file '" << d_file_path << "'.";
	  setError(Error::SetFilePosIndicator, stream.str());
	  return;
	}
  
      std::string current_value;
      file >> current_value;
      if (file.fail())
	{
	  std::ostringstream stream("setParam - Failed to extract the parameter value of the parameter '",
				    std::ios_base::app);
	  stream << name_match << "' from the file '" << d_file_path << "'.";
	  setError(Error::ParamExtraction, stream.str());
	  return;
	}

      if (current_value == new_value)
	{
	  // nothing is needed to be done if the values are the same
	  return;
	}

      // if the new content has a smaller length than the current content, then replace and pad to make the length the same, if necessary.
      int length_diff = current_value.length()-new_value.length();
      if (length_diff >= 0)
	{
	  file.seekp(val_pos); // set output pos indicator
	  if (file.fail())
	    {
	      std::ostringstream stream("setParam - Unexpected failure when setting the output position indicator to ",
					std::ios_base::app);
	      stream << val_pos << "' in the file '" << d_file_path << "'.";
	      setError(Error::SetFilePosIndicator, stream.str());
	      return;
	    }

	  file << new_value;
	  for (int i=0; i<length_diff; ++i)
	    {
	      file << " ";
	    }
	  if (file.fail())
	    {
	      std::ostringstream stream("setParam - Unexpected failure when outputting the new parameter value ' ",
					std::ios_base::app);
	      stream << new_value << "' with " << length_diff << " padded spaces into the file '" << d_file_path << "'.";
	      setError(Error::OutputToFile, stream.str());
	      return;
	    }
	  return;
	}
  
      // If the new content is longer in length than the current content, then copy and paste data:
      // Copy all of the file starting from the end of the parameter value
      file.seekg(val_pos+static_cast<std::streamoff>(current_value.length())); // set input pos indicator
      if (file.fail())
	{
	  std::ostringstream stream("setParam - Unexpected failure when setting the input position indicator to ",
				    std::ios_base::app);
	  stream << val_pos+static_cast<std::streamoff>(current_value.length()) << " in the file" << d_file_path << "'.";
	  setError(Error::SetFilePosIndicator, stream.str());
	  return;
	}
      std::stringstream trailing_data;
      trailing_data << file.rdbuf();
      // Set the new parameter value and trailing data
      // First reset the internal basic_stringbuf position of the new file data after the new parameter
      trailing_data.seekp(0);
      trailing_data.seekg(0);
      file.seekp(val_pos); // set output pos indicator
      file << new_value;
      file << trailing_data.rdbuf();
      if (file.fail())
	{
	  std::ostringstream stream("setParam - Unexpected failure when outputting the new file data to the file '",
				    std::ios_base::app);
	  stream << d_file_path << "'.";
	  setError(Error::OutputToFile, stream.str());
	  return;
	}
    }
  catch (...)
    {
      std::ostringstream stream("setParam - Unexpected exception while trying to set the new parameter value in the file '",
				std::ios_base::app);
      stream << d_file_path << "'.";
      setError(Error::SetParamException, stream.str());
      return;
    }
  return;
}

//----------------------------------------------------------------------//
bool FileParamManager::removeQuotesFromTo(const std::string& orig,
					  std::string& no_quotes)
{
  // strcmp returns 0 if the strings are the same
  static constexpr char quote_one[] = "\'";
  static constexpr char quote_two[] = "\"";
  if ((orig.front() != quote_one[0] &&
       orig.front() != quote_two[0]) ||
      (orig.back() != quote_one[0] &&
       orig.back() != quote_two[0]))
    {
      return false;
    }

  try
    {
      no_quotes = orig.substr(1,orig.length()-2);
    }
  catch (...)
    {
      assert(false);
      return false;
    }
  return true;
}

//----------------------------------------------------------------------//
std::iostream::pos_type FileParamManager::getParamValuePos(std::fstream& file,
							   const std::string& name_match)
{
  file.seekg(0); // reset input stream position indicator

  std::string line;
  size_t line_match_pos = 0;
  std::iostream::pos_type previous_ipos = 0;

  try
    {
      while (1)
	{
	  previous_ipos = file.tellg();
	  std::getline(file, line);

	  line_match_pos = line.find(name_match);
	  if (line_match_pos != std::string::npos)
	    {
	      break; // found!
	    }
	  
	  if (file.eof())
	    {
	      std::ostringstream stream("getParamValuePos - The parameter '",
					std::ios_base::app);
	      stream << name_match << "' does not exist in the file '" << d_file_path << "'.";
	      setError(Error::ParamNotFound,stream.str());
	      return std::iostream::pos_type(-1);
	    }
	  
	  if (file.fail())
	    {
	      std::ostringstream stream("getParamValuePos - Failed to extract characters from the file '",
					std::ios_base::app);
	      stream << d_file_path << "'.";
	      setError(Error::FileCharacterExtraction, stream.str());
	      return std::iostream::pos_type(-1);
	    }
	}
    }
  catch (...)
    {
      std::ostringstream stream("getParamValuePosException - Unexpected exception while trying to get the position index of the start of the value of the parameter '",
				std::ios_base::app);
      stream << name_match << "' in the file '" << d_file_path << "'.";
      setError(Error::GetParamValuePosException, stream.str());
      return false;
    }
  size_t line_value_pos = line_match_pos+name_match.length();
  return previous_ipos+static_cast<std::streamoff>(line_value_pos);
}

//----------------------------------------------------------------------//
void FileParamManager::goToParamValue(std::fstream& file,
				      const std::string& name_match)
{
  std::iostream::pos_type val_pos = getParamValuePos(file, name_match);
  if (hasError())
    {
      std::ostringstream stream("goToParamValue - Failed to go to the parameter value of the parameter '",
				std::ios_base::app);
      stream << name_match << "' in the file '"
	     << d_file_path << "' because of the following error:\n"
	     << getErrorMsg();
      setError(Error::ParamNotFound,stream.str());
      return;
    }
  file.seekg(val_pos); // set input indicator
  file.seekp(val_pos); // set output indicator
}

//----------------------------------------------------------------------//
void FileParamManager::setError(Error e, const std::string& msg)
{
  d_error = e;
  d_error_msg = "CORE::FileParamManager::";
  d_error_msg += msg;
}
