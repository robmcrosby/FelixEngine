//
//  EventSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/27/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef EventSystem_h
#define EventSystem_h

#include "System.h"
#include "Mutex.h"


namespace fx
{
  class FelixEngine;
  
  class EventSystem: public System
  {
  public:
    static EventSystem* Instance() {return sInstance;}
    
    EventSystem(): System(SYSTEM_EVENTS) {}
    virtual ~EventSystem() {}
    
    virtual void pollEvents() = 0;
    
  private:
    static EventSystem *sInstance;
  };
}

#endif /* EventSystem_h */
