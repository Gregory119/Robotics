#include "core_fileparamextractor.h"

#include <cassert>

using namespace CORE;

//----------------------------------------------------------------------//
FileParamExtractor::FileParamExtractor(std::string file_path)
  : d_file_path(std::move(file_path))
{}

//----------------------------------------------------------------------//
std::string FileParamExtractor::getParam(const std::string& name_match)
{
  std::string empty_str;
  if (hasError())
    {
      // LOG!!!!!
      assert(false);
      return empty_str;
    }
  
  std::ifstream file(d_file_path);

  if (file.fail())
    {
      assert(false);
      std::ostringstream stream("getParam - Failed to open the file '",
				std::ios_base::app);
      stream << d_file_path << "'.";
      setError(Error::OpenFile, stream.str());
      return empty_str;
    }

  std::string line;
  size_t pos = 0;

  while (1)
    {
      std::getline(file, line);

      if (file.fail())
	{
	  assert(false);
	  std::ostringstream stream("getParam - Failed to extract characters from the file '",
				    std::ios_base::app);
	  stream << d_file_path << "'.";
	  setError(Error::FileCharacterExtraction, stream.str());
	  return empty_str;
	}
      
      pos = line.find(name_match);
      if (pos != std::string::npos)
	{
	  break; // found!
	}

      if (file.eof())
	{
	  std::ostringstream stream("getParam - The parameter '",std::ios_base::app);
	  stream << name_match << "' does not exist in the file '" << d_file_path << "'.";
	  setError(Error::ParamNotFound,stream.str());
	  return empty_str;
	}
    }
  
  try
    {
      std::string value = line.substr(pos+name_match.size());
      if (!value.empty())
	{
	  return value;
	}
    }
  catch (...)
    {
      // just continue to error condition
    }
  std::ostringstream stream("getParam - Failed to extract the parameter string from '",
			    std::ios_base::app);
  stream << line << "'.";
  setError(Error::ParamExtraction, stream.str());
  return empty_str;
}

//----------------------------------------------------------------------//
void FileParamExtractor::setError(Error e, const std::string& msg)
{
  d_error = e;
  d_error_msg = "FileParamExtractor::";
  d_error_msg += msg;
}
