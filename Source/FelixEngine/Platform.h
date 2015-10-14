//
//  Platform.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Platform_h
#define Platform_h

#include <iostream>

namespace fx
{
  /**
   *
   */
  struct Platform
  {
    static std::string GetResourcePath();
    static std::string LoadTextFile(const std::string &filePath);
    
    static long ReadUints(unsigned int *buffer, long size, std::istream &is);
    static long ReadUint(unsigned int &i, std::istream &is) {return ReadUints(&i, 1, is);}
    
    static long ReadInts(int *buffer, long size, std::istream &is) {return ReadUints((unsigned int*)buffer, size, is);}
    static long ReadInt(int &i, std::istream &is) {return ReadUints((unsigned int*)&i, 1, is);}
    
    static long ReadFloats(float *buffer, long size, std::istream &is) {return ReadUints((unsigned int*)buffer, size, is);}
    static long ReadFloat(float &f, std::istream &is) {return ReadUints((unsigned int*)&f, 1, is);}
    
    static unsigned int GetTimeStamp();
  };
}


#endif /* Platform_h */
