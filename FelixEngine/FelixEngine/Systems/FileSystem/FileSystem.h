//
//  FileSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef FileSystem_h
#define FileSystem_h

#include <stdio.h>

namespace fx {
  
  /** File System */
  class FileSystem {
  protected:
    static FileSystem *instance;
    
  public:
    virtual ~FileSystem() {}
  };
  
}

#endif /* FileSystem_h */