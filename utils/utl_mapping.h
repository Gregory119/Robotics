#ifndef UTIL_MAPPPING_H
#define UTIL_MAPPPING_H

#include <type_traits>
#include <cassert>

namespace UTIL
{
  class Map
  {
  public:
    Map() = default;
  Map(double in_max, double in_min, double o_max, double o_min, double o_mid)
    : d_in_max(in_max),
      d_in_min(in_min),
      d_in_mid((in_max+in_min)/2),
      d_out_max(o_max),
      d_out_min(o_min),
      d_out_mid(o_mid)
    {}
    Map(double in_max, double in_min, double o_max, double o_min); // most common
    Map(const Map&) = default;
    Map& operator=(const Map&) = default;
    Map(Map&&) = default;
    Map& operator=(Map&&) = default;

    void setInputRange(double in_max, double in_min);
    void setOutputRange(double out_max, double out_min);
    void setOutputMiddle(double out_mid);

    template <class T>
    void map(double in_val, T& out_val) const;
    template <class T>
    T map(double in_val) const;

    // The input value here is actually in the units of the internal output members,
    // and the output value here is actually in the units of the internal input members.
    template <class T>
    void inverseMap(double in_val, T& out_val) const;
    template <class T>
    T inverseMap(double in_val) const;

    template <class T>
      T flipOnOutAxis(T) const;
    template <class T>
      T flipOnInAxis(T) const;
    
  private:
    double d_in_max = 0;
    double d_in_min = 0;
    double d_in_mid = 0;
    double d_out_max = 0;
    double d_out_min = 0;
    double d_out_mid = 0;
  };
  
  //----------------------------------------------------------------------//
  template <class T>
    void Map::map(double in_val, T& out_val) const
    {
      if (!std::is_fundamental<T>::value)
	{
	  assert(false);
	  //LOG!!!
	  return;
	}

      if (in_val > d_in_max || in_val < d_in_min)
	{
	  assert(false);
	  //LOG!!!
	  return;
	}

      if (in_val >= d_in_mid)
	{
	  out_val = (in_val - d_in_mid)/ 
	    (d_in_max - d_in_mid)*
	    (d_out_max - d_out_mid) + d_out_mid;
	  assert(out_val >= d_out_mid);
	}
      else
	{
	  out_val = (in_val - d_in_min)/ 
	    (d_in_mid - d_in_min)*
	    (d_out_mid - d_out_min) + d_out_min;
	  assert(out_val < d_out_mid);
	}

      assert(out_val <= d_out_max);
      assert(out_val >= d_out_min);
    }

  //----------------------------------------------------------------------//
  template <class T>
    T Map::map(double in_val) const
    {
      T out_val;
      map(in_val, out_val);
      return out_val;
    }
  
  //----------------------------------------------------------------------//
  template <class T>
    void Map::inverseMap(double in_val, T& out_val) const
    {
      // The input value here is actually in the units of the internal output members,
      // and the output value here is actually in the units of the internal input members.
      if (!std::is_fundamental<T>::value)
	{
	  assert(false);
	  //LOG!!!
	  return;
	}
      
      if (in_val > d_out_max || in_val < d_out_min)
	{
	  assert(false);
	  //LOG!!!
	  return;
	}

      if (in_val >= d_out_mid)
	{
	  out_val = (in_val - d_out_mid)/ 
	    (d_out_max - d_out_mid)*
	    (d_in_max - d_in_mid) + d_in_mid;
	  assert(out_val >= d_in_mid);
	}
      else
	{
	  out_val = (in_val - d_out_min)/ 
	    (d_out_mid - d_out_min)*
	    (d_in_mid - d_in_min) + d_in_min;
	  assert(out_val < d_in_mid);
	}

      assert(out_val <= d_in_max);
      assert(out_val >= d_in_min);
    }

  //----------------------------------------------------------------------//
  template <class T>
    T Map::inverseMap(double in_val) const
    {
      T out_val;
      inverseMap(in_val, out_val);
      return out_val;
    }
  
  //----------------------------------------------------------------------//
  template <class T>
    T Map::flipOnOutAxis(T val) const
    {
      if (val > d_out_max || val < d_out_min)
	{
	  assert(false);
	  //LOG!!!
	}
      return (d_out_max - val) + d_out_min;
    }

  //----------------------------------------------------------------------//
  template <class T>
    T Map::flipOnInAxis(T val) const
    {
      if (val > d_in_max || val < d_in_min)
	{
	  assert(false);
	  //LOG!!!
	  return;
	}
      return (d_in_max - val) + d_in_min;
    }

};
#endif
