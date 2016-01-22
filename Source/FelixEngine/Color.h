//
//  Color.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Color_h
#define Color_h

#include <iostream>

namespace fx
{
  /**
   *
   */
  enum FILTER_TYPE {
    FILTER_MIN_LINEAR =     0x01,
    FILTER_MAG_LINEAR =     0x02,
    FILTER_MIPMAP_NEAREST = 0x04,
    FILTER_MIPMAP_LINEAR =  0x08,
  };
  
  /**
   *
   */
  enum COLOR_TYPE {
    COLOR_RGBA,
    COLOR_FLOAT,
    COLOR_DEPTH32F,
    COLOR_DEPTH24,
    COLOR_STENCIL8,
    COLOR_NONE,
  };
  
  static COLOR_TYPE ParseColorType(const std::string &str)
  {
    if (str == "rgba")
      return COLOR_RGBA;
    if (str == "float")
      return COLOR_FLOAT;
    if (str == "depth")
      return COLOR_DEPTH32F;
    if (str == "stencil")
      return COLOR_STENCIL8;
    return COLOR_NONE;
  }
  
  template <typename T>
  struct Color4
  {
    Color4(): red(0), green(0), blue(0), alpha(0) {}
    Color4(T r, T g, T b, T a): red(r), green(g), blue(b), alpha(a) {}
    Color4(const std::string &str) {parse(str);}
    
    const void* ptr() const {return (const void*)&red;}
    bool parse(const std::string &str)
    {
      float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
      int res = sscanf(str.c_str(), " %f , %f , %f , %f", &r, &g, &b, &a);
      
      red   = (T)r;
      green = (T)g;
      blue  = (T)b;
      alpha = (T)a;
      return res = 4;
    }
    
    T red, green, blue, alpha;
  };
  
  typedef Color4<unsigned char> RGBA;
  typedef Color4<float> RGBAf;
}

#endif /* Color_h */
