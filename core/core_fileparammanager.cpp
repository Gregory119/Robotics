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
void FileParamManager::enableFailOnDuplicateParams()
{
  d_check_duplicate_params = true;
}

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
  catch (const std::exception& e)
    {
      std::ostringstream stream("getParam - Failed to extract the parameter value of the parameter '",
				std::ios_base::app);
      stream << name_match << "' from the file '" << d_file_path << "' because of the exception '" << e.what() << ".";
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
      if (hasError())
	{
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
  catch (const std::exception& e)
    {
      std::ostringstream stream("setParam - Unexpected exception '",
				std::ios_base::app);
      stream << e.what() << "' while trying to set the new parameter value in the file" << d_file_path << "'.";
      setError(Error::SetParamException, stream.str());
      return;
    }
  return;
}

//----------------------------------------------------------------------//
std::string FileParamManager::str()
{
  std::fstream file(d_file_path);

  if (file.fail())
    {
      std::ostringstream stream("str - Failed to open the file '",
				std::ios_base::app);
      stream << d_file_path << "'.";
      setError(Error::FileOpen, stream.str());
      return std::string();
    }
  
  std::string out;
  std::getline(file, out, file.widen(std::char_traits<char>::eof()));
  return out;
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

  std::iostream::pos_type ret = -1;
  std::string line;
  size_t line_match_pos = std::string::npos;
  std::iostream::pos_type first_match_previous_ipos = 0;

  try
    {
      while (1)
	{
	  if (line_match_pos == std::string::npos) // not found yet
	    {
	      first_match_previous_ipos = file.tellg();
	    }
	  
	  std::getline(file, line);

	  if (line_match_pos == std::string::npos) // not found yet
	    {
	      line_match_pos = line.find(name_match);
	      if (line_match_pos != std::string::npos &&
		  !d_check_duplicate_params)
		{
		  break;
		}
	    }
	  // has been found but if..
	  else if (line.find(name_match) != std::string::npos)
	    {
	      std::ostringstream stream("getParamValuePos - The parameter '",
					std::ios_base::app);
	      stream << name_match << "' was found multiple times in the file '" << d_file_path << "'. The parameter value must only be defined once.";
	      setError(Error::MultiParamDef, stream.str());
	      return ret;
	    }

	  if (file.fail())
	    {
	      if (line_match_pos != std::string::npos)
		{
		  // found only one match in the entire file
		  break;
		}
	      std::ostringstream stream("getParamValuePos - Failed to find the parameter '",
					std::ios_base::app);
	      stream << name_match << "' in the file '" << d_file_path << "'.";
	      setError(Error::ParamNotFound, stream.str());
	      return ret;
	    }
	}
    }
  catch (const std::exception& e)
    {
      std::ostringstream stream("getParamValuePosException - Unexpected exception '",
				std::ios_base::app);
      stream << e.what() << "' while trying to get the position index of the start of the value of the parameter '" << name_match << "' in the file '" << d_file_path << "'.";
      setError(Error::GetParamValuePosException, stream.str());
      return ret;
    }
  file.seekg(0); // reset input stream position indicator
  file.clear(); // clear the failed state due to reaching the eof
  size_t line_value_pos = line_match_pos+name_match.length();
  return first_match_previous_ipos+static_cast<std::streamoff>(line_value_pos);
}

//----------------------------------------------------------------------//
void FileParamManager::goToParamValue(std::fstream& file,
				      const std::string& name_match)
{
  std::iostream::pos_type val_pos = getParamValuePos(file, name_match);

  if (hasError())
    {
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
