//
//  StringUtils.h
//  FelixEngine
//
//  Created by Robert Crosby on 1/21/15.
//  Copyright (c) 2015 Robert Crosby. All rights reserved.
//

#ifndef FelixEngine_StringUtils_h
#define FelixEngine_StringUtils_h

#include <string>
#include <sstream>
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
    static void ToLower(std::string &str)
    {
      std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    }
    
    /**
     * Converts a string to upper case.
     *
     * @param str  String to upper case
     */
    static void ToUpper(std::string &str)
    {
      std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    }
    
    /**
     * Extracts the file name from a file path string.
     *
     * @param path  String for the path.
     * @param name  String for the file name.
     */
    static bool GetFileNameFromPath(const std::string &path, std::string &name)
    {
      bool success = false;
      std::string::size_type start, end, npos = std::string::npos;
      
      start = path.find_last_of('/');
      if (start == npos)
        start = 0;
      else
        start++;
      
      end = path.find_last_of('.');
      
      if (end != npos && start < end)
      {
        name = path.substr(start, end-start);
        success = true;
      }
      
      return success;
    }
    
    /**
     * Parses an intenger from the given string.
     *
     * @param value   Intenger refrence to assign the parsed value to.
     * @param str     String to parse the intenger from.
     * @return        True if parsed successfully read, False otherwise.
     */
    static bool ParseIntenger(int &value, const std::string &str)
    {
      return sscanf(str.c_str(), " %i", &value) == 1;
    }
    
    /**
     * Parses a float from the given string.
     *
     * @param value   Float refrence to assign the parsed value to.
     * @param str     String to parse the float from.
     * @return        True if parsed successfully read, False otherwise.
     */
    static bool ParseFloat(float &value, const std::string &str)
    {
      return sscanf(str.c_str(), " %f", &value) == 1;
    }
    
    /**
     * Parses comma separated floats from the given string.
     *
     * @param values  Vector of Floats assign the parsed values to.
     * @param str     String to parse the floats from.
     */
    static void ParseFloats(std::vector<float> &values, const std::string &str)
    {
      std::stringstream ss(str);
      std::string line;
      
      while (getline(ss, line, ','))
      {
        float value;
        if (fx::StringUtils::ParseFloat(value, line))
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
    static bool ParseInt(int &value, const std::string &str)
    {
      return sscanf(str.c_str(), " %i", &value) == 1;
    }
    
    /**
     * Parses comma separated integers from the given string.
     *
     * @param values  Vector of integers assign the parsed values to.
     * @param str     String to parse the integers from.
     */
    static void ParseInts(std::vector<int> &values, const std::string &str)
    {
      std::stringstream ss(str);
      std::string line;
      
      while (getline(ss, line, ','))
      {
        int value;
        if (fx::StringUtils::ParseInt(value, line))
          values.push_back(value);
      }
    }
    
    /**
     *
     * @param fileName  String to extract the file postfix from.
     * @return          Either the postfix string or an empty string if none found.
     */
    static std::string GetFilePostfix(const std::string &fileName)
    {
      size_t loc = fileName.rfind(".");
      if (loc != std::string::npos)
        return fileName.substr(loc+1);
      return "";
    }
  };
}

#endif
