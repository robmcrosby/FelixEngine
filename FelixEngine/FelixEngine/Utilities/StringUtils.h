//
//  StringUtils.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef StringUtils_h
#define StringUtils_h

#include <string>
#include <vector>


namespace fx
{
  struct StringUtils
  {
    /**
     * Converts a string to lower case.
     *
     * @param str  String to upper case
     */
    static void toLower(std::string &str)
    {
      std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    }
    
    /**
     * Converts a string to upper case.
     *
     * @param str  String to upper case
     */
    static void toUpper(std::string &str)
    {
      std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    }
    
    /**
     * Parses a float from the given string.
     *
     * @param value   Float refrence to assign the parsed value to.
     * @param str     String to parse the float from.
     * @return        True if parsed successfully read, False otherwise.
     */
    static bool parseFloat(float &value, const std::string &str)
    {
      return sscanf(str.c_str(), " %f", &value) == 1;
    }
    
    /**
     * Parses comma separated floats from the given string.
     *
     * @param values  Vector of Floats assign the parsed values to.
     * @param str     String to parse the floats from.
     */
    static void parseFloats(std::vector<float> &values, const std::string &str)
    {
      std::stringstream ss(str);
      std::string line;
      
      while (getline(ss, line, ','))
      {
        float value;
        if (fx::StringUtils::parseFloat(value, line))
          values.push_back(value);
      }
    }
    
    /**
     * Parses an integer from the given string.
     *
     * @param value   Int refrence to assign the parsed value to.
     * @param str     String to parse the float from.
     * @return        True if parsed successfully read, False otherwise.
     */
    static bool parseInt(int &value, const std::string &str)
    {
      return sscanf(str.c_str(), " %i", &value) == 1;
    }
    
    /**
     * Parses comma separated integers from the given string.
     *
     * @param values  Vector of integers assign the parsed values to.
     * @param str     String to parse the integers from.
     */
    static void parseInts(std::vector<int> &values, const std::string &str)
    {
      std::stringstream ss(str);
      std::string line;
      
      while (getline(ss, line, ','))
      {
        int value;
        if (fx::StringUtils::parseInt(value, line))
          values.push_back(value);
      }
    }
  };
}

#endif /* StringUtils_h */
