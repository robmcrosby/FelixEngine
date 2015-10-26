//
//  TaskingSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef TaskingSystem_h
#define TaskingSystem_h

#include "System.h"

namespace fx
{
  class TaskingSystem: public System
  {
  public:
    TaskingSystem(): System(SYSTEM_TASKING) {}
    virtual ~TaskingSystem() {}
  };
}

#endif /* TaskingSystem_h */
