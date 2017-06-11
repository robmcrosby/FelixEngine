//
//  UbiquityFileSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/6/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UbiquityFileSystem_h
#define UbiquityFileSystem_h

#include "FileSystem.h"
#include "Condition.h"

namespace fx
{
  class UbiquityFileSystemInfo;
  
  /**
   *
   */
  class UbiquityFileSystem: public FileSystem
  {
  public:
    UbiquityFileSystem();
    virtual ~UbiquityFileSystem();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    virtual File getDocuments();
    
    void handleUpdate();
    static UbiquityFileSystem *Instance() {return UbiquityFileSystem::sInstance;}
    
  private:
    void initUbiquity(void*);
    File getUbiquity();
    
    Condition mCondition;
    UbiquityFileSystemInfo *mInfo;
    
    static UbiquityFileSystem *sInstance;
  };
}

#endif /* UbiquityFileSystem_h */
