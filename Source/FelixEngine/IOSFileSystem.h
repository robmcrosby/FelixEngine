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
#include "Condition.h"

namespace fx
{
  class IOSFileSystemInfo;
  
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
    
    virtual File getDocuments();
    
    void handleUpdate();
    static IOSFileSystem *Instance() {return IOSFileSystem::sInstance;}
    
  private:
    void initUbiquity(void*);
    File getUbiquity();
    
    Condition mCondition;
    IOSFileSystemInfo *mInfo;
    
    static IOSFileSystem *sInstance;
  };
}

#endif /* IOSFileSystem_h */
