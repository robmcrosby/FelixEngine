//
//  EventHandler.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/28/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef EventHandler_h
#define EventHandler_h

#include "Event.h"
#include "TaskGroup.h"


namespace fx
{
  class EventHandler
  {
  public:
    EventHandler() {}
    virtual ~EventHandler() {}
    
    virtual void handle(Event event) {}
    
    void dispatchSerial(Event event) {}
    void dispatchParrallel(Event event) {}
    void dispatchAndWait(Event event) {}
    
    
  private:
    struct Observer
    {
      Observer(EventHandler *h = nullptr): handler(h) {}
      EventHandler *handler;
    };
  };
}

#endif /* EventHandler_h */
