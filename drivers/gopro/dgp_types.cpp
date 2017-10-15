#include "dgp_types.h"
#include "dgp_utils.h"

#include <cassert>

using namespace D_GP;

//----------------------------------------------------------------------//
bool Status::loadStr(const std::string& body, CamModel model)
{
  switch (model)
    {
      case CamModel::Hero5:
	return loadStrHero5(body);
    };
  assert(false);
  return false;
}

//----------------------------------------------------------------------//
bool Status::loadStrHero5(const std::string& body)
{
  clear();
  if (body.empty())
    {
      return false;
    }

  // all appear first in the 'status' JSON node
  size_t pos_mode = body.find("\"43\":");
  size_t pos_sub_mode = body.find("\"44\":"); 
  size_t pos_rec = body.find("\"8\":");
  size_t pos_stream = body.find("\"32\":"); 

  if (pos_mode == std::string::npos ||
      pos_sub_mode == std::string::npos ||
      pos_rec == std::string::npos ||
      pos_stream == std::string::npos)
    {
      return false;
    }

  try
    {
      int mode = -1;
      try
	{
	  mode = std::stoi(body.substr(pos_mode+5,1));
	}
      catch (...)
	{
	  return false;
	}

      int sub_mode = -1;
      try
	{
	  sub_mode = std::stoi(body.substr(pos_mode+5,1));
	}
      catch (...)
	{
	  return false;
	}
      
      d_mode = Utils::extractModeStatus(mode, sub_mode);
      if (d_mode == Mode::Unknown)
	{
	  // already in cleared state
	  return false;
	}
      
      int rec = std::stoi(body.substr(pos_rec+5,1));
      if (rec != 1 && rec != 0)
	{
	  clear();
	  return false;
	}
      d_is_recording = (rec != 0);

      int stream = std::stoi(body.substr(pos_stream+5,1));
      if (stream != 1 && stream != 0)
	{
	  clear();
	  return false;
	}
      d_is_streaming = (stream != 0);
    }
  catch (...)
    {
      clear();
      return false;
    }
  
  return true;
}
