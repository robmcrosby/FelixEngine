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


namespace fx
{
  class EventSystem: public System
  {
  public:
    static EventSystem* Instance() {return sInstance;}
    
    EventSystem(): System(SYSTEM_EVENTS)
    {
      sInstance = this;
    }
    virtual ~EventSystem() {}
    
    virtual bool setToXml(const XMLTree::Node *node) {return true;}
    virtual bool init() {return true;}
    
  private:
    static EventSystem *sInstance;
  };
}

#endif /* EventSystem_h */
