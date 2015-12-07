//
//  IOSFileSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/6/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef IOSFileSystem_h
#define IOSFileSystem_h

#include "FileSystem.h"

namespace fx
{
  /**
   *
   */
  class IOSFileSystem: public FileSystem
  {
  public:
    IOSFileSystem();
    virtual ~IOSFileSystem();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
  };
}

#endif /* IOSFileSystem_h */
