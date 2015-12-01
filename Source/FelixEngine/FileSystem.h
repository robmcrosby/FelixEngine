//
//  FileSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/1/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef FileSystem_h
#define FileSystem_h

#include "System.h"

namespace fx
{
  /**
   *
   */
  class FileSystem: public System
  {
  public:
    static std::string GetLocalPath();
    static bool LoadText(std::string &buffer, const std::string &filePath);
    static std::string LoadText(const std::string &filePath);
    
    static long Read(unsigned int *buffer, long size, std::istream &is);
    static long Read(unsigned int &i, std::istream &is) {return Read(&i, 1, is);}
    
    static long Read(int *buffer, long size, std::istream &is) {return Read((unsigned int*)buffer, size, is);}
    static long Read(int &i, std::istream &is) {return Read((unsigned int*)&i, 1, is);}
    
    static long Read(float *buffer, long size, std::istream &is) {return Read((unsigned int*)buffer, size, is);}
    static long Read(float &f, std::istream &is) {return Read((unsigned int*)&f, 1, is);}
    
  public:
    virtual ~FileSystem() {}
  };
}

#endif /* FileSystem_h */
