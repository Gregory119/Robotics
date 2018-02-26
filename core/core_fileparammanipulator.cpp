#include "core_fileparammanipulator.h"

#include <cassert>
#include <fstream>
#include <sstream>

using namespace CORE;

//----------------------------------------------------------------------//
FileParamManipulator::FileParamManipulator(std::string file_path)
  : d_file_path(std::move(file_path)),
    d_file(d_file_path)
{
  if (d_file.fail())
    {
      assert(false);
      std::ostringstream stream("FileParamManipulator - Failed to open the file '",
				std::ios_base::app);
      stream << d_file_path << "'." << std::endl;
      setError(Error::FileOpen, stream.str());
      return;
    }
}

//----------------------------------------------------------------------//
std::string FileParamManipulator::getParam(const std::string& name_match)
{
  std::string empty_str;
  if (hasError())
    {
      // LOG!!!!!
      assert(false);
      return empty_str;
    }

  goToParamValue(name_match);
  std::string value;
  d_file >> value;

  if (d_file.fail())
    {
      std::ostringstream stream("getParam - Failed to extract the parameter string from '",
				std::ios_base::app);
      stream << line << "'." << std::endl;
      setError(Error::ParamExtraction, stream.str());
      return empty_str;
    }
  
  d_file.seekg(0); // reset input stream position indicator
  d_file.seekp(0); // reset output stream position indicator
  return value;
}

//----------------------------------------------------------------------//
bool FileParamManipulator::setParam(const std::string& name_match,
				    const std::string& new_value)
{
  if (hasError())
    {
      // LOG!!!!!
      assert(false);
      return false;
    }
  
  // search and get the parameter value and value position
  std::iostream::pos_type val_pos = getParamValuePos(name_match);
  d_file.seekg(val_pos); //set input pos indicator
  std::string current_value;
  d_file >> current_value;

  if (d_file.fail())
    {
      std::ostringstream stream("setParam - Failed to extract the parameter value string of '",
				std::ios_base::app);
      stream << name_match << "'." << std::endl;
      setError(Error::ParamExtraction, stream.str());
      return false;
    }
  
  if (current_value == new_value)
    {
      std::ostringstream stream("setParam - Setting the parameter value to the same value is not accepted. The parameter value is '",
				std::ios_base::app);
      stream << current_value << "'." << std::endl;
      setError(Error::ParamUnchanged, stream.str());
      return false;
    }

  // if the new content has a smaller length than the current content, then replace and pad to make the length the same, if necessary.
  size_t length_diff = current_value.length()-new_value.length();
  if (length_diff >= 0)
    {
      d_file.seekp(val_pos); // set output pos indicator
      d_file << new_value;

      for (unsigned i=0; i<length_diff; ++i)
	{
	  d_file << " ";
	}
      
      return true;
    }
  
  // If the new content is longer in length than the current content, then create a new file with the new data:
  // First set the position to the start of the parameter value.
  // Copy 
  d_file.seekg(val_pos); // set input pos indicator
  std::ostringstream new_file_data;
  new_file_data << new_value;

  // to create a new file:
  // copy file contents up to the end of the match to a temporary output file
  // output the new parameter value to the temporary file
  // copy the rest of the original file, excluding the parameter value, into the temporary file
  // delete the original file and rename the temporary file to the original file name
}

//----------------------------------------------------------------------//
std::iostream::pos_type FileParamManipulator::getParamValuePos(const std::string& name_match)
{
  std::string line;
  size_t line_match_pos = 0;
  std::iostream::pos_type previous_ipos = 0;

  while (1)
    {
      previous_ipos = file.tellg();
      std::getline(d_file, line);

      if (d_file.fail())
	{
	  assert(false);
	  std::ostringstream stream("getParamValuePos - Failed to extract characters from the file '",
				    std::ios_base::app);
	  stream << d_file_path << "'." << std::endl;
	  setError(Error::FileCharacterExtraction, stream.str());
	  return std::iostream::pos_type(-1);
	}
      
      line_match_pos = line.find(name_match);
      if (line_match_pos != std::string::npos)
	{
	  break; // found!
	}

      if (d_file.eof())
	{
	  std::ostringstream stream("getParamValuePos - The parameter '",std::ios_base::app);
	  stream << name_match << "' does not exist in the file '" << d_file_path << "'." << std::endl;
	  setError(Error::ParamNotFound,stream.str());
	  return std::iostream::pos_type(-1);
	}
    }

  d_file.seekg(0); // reset input stream position indicator
  d_file.seekp(0); // reset output stream position indicator
  size_t line_value_pos = line_match_pos+name_match.length();
  return previous_ipos+static_cast<std::streamoff>(line_value_pos);
}

//----------------------------------------------------------------------//
void FileParamManipulator::goToParamValue(const std::string& name_match)
{
  getParamValuePos(name_match);
  if (hasError())
    {
      std::ostringstream stream("goToParamValue - Failed to go to the parameter '",std::ios_base::app);
      stream << name_match << "' in the file '" << d_file_path << "'." << std::endl;
      setError(Error::ParamNotFound,stream.str());
      return;
    }
  
  d_file.seekg();
}

//----------------------------------------------------------------------//
void FileParamManipulator::setError(Error e, const std::string& msg)
{
  d_error = e;
  d_error_msg = "FileParamManipulator::";
  d_error_msg += msg;
}
