//
//  GCDTaskingSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GCDTaskingSystem_h
#define GCDTaskingSystem_h

#include "TaskingSystem.h"


namespace fx
{
  class GCDTaskingSystem: public TaskingSystem
  {
  public:
    GCDTaskingSystem();
    virtual ~GCDTaskingSystem();
    
    void update(void*);
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
  };
}

#endif /* GCDTaskingSystem_h */
