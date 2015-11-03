//
//  SDLEventSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/3/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef SDLEventSystem_h
#define SDLEventSystem_h

#include "EventSystem.h"


namespace fx
{
  class FelixEngine;
  
  class SDLEventSystem: public EventSystem
  {
  public:
    SDLEventSystem();
    virtual ~SDLEventSystem();
    
    virtual void pollEvents();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
  private:
    FelixEngine *mEngine;
    Mutex mUpdateMutex;
  };
}

#endif /* SDLEventSystem_h */
