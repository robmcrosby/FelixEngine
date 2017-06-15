//
//  iOSFileSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "FileSystem.h"

namespace fx {
  
  /** iOS File System */
  class iOSFileSystem: public FileSystem {
  public:
    iOSFileSystem();
    virtual ~iOSFileSystem();
  };
  
}
