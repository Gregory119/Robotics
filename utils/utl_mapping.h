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
  Map(double in_max, double in_min, double o_max, double o_min)
    : d_in_max(in_max),
      d_in_min(in_min),
      d_out_max(o_max),
      d_out_min(o_min)
    {}
    Map(const Map &copy) = default;
    Map& operator=(const Map &copy) = default;

    void setInputRange(double in_max, double in_min);
    void setOutputRange(double out_max, double out_min);

    template <class T>
    void map(double in_val, T& out_val) const;
    template <class T>
    void inverseMap(double in_val, T& out_val) const;
    template <class T>
      T flipOnOutAxis(T) const;
    template <class T>
      T flipOnInAxis(T) const;
    
  private:
    double d_in_max = 0;
    double d_in_min = 0;
    double d_out_max = 0;
    double d_out_min = 0;
  };
  
  //----------------------------------------------------------------------//
  // Flip axis will swap the output max and min
  // Only for fundamental types
  template <class T>
    void Map::map(double in_val, T& out_val) const
    {
      assert(std::is_fundamental<T>::value);
      assert(in_val <= d_in_max);
      assert(in_val >= d_in_min);
      
      out_val = (in_val - d_in_min)/ 
	(d_in_max - d_in_min)*
	(d_out_max - d_out_min) + d_out_min;

      assert(out_val <= d_out_max);
      assert(out_val >= d_out_min);
    }

  //----------------------------------------------------------------------//
  // Flip axis will swap the output max and min
  // Only for fundamental types
  template <class T>
    void Map::inverseMap(double in_val, T& out_val) const
    {
      assert(std::is_fundamental<T>::value);
      assert(in_val <= d_out_max);
      assert(in_val >= d_out_min);
      
      out_val = (in_val - d_out_min)/ 
	(d_out_max - d_out_min)*
	(d_in_max - d_in_min) + d_in_min;

      assert(out_val <= d_in_max);
      assert(out_val >= d_in_min);
    }

  //----------------------------------------------------------------------//
  template <class T>
    T Map::flipOnOutAxis(T val) const
    {
      return (d_out_max - val) + d_out_min;
    }

  //----------------------------------------------------------------------//
  template <class T>
    T Map::flipOnInAxis(T val) const
    {
      return (d_in_max - val) + d_in_min;
    }

};
#endif
