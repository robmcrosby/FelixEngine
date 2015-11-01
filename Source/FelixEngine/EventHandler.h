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
#include "Pool.h"
#include "MultiVector.h"
#include "TaskGroup.h"
#include "List.h"


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
    typedef EventHandler** Observer;
    static Pool<Observer> ObserverPool;
    static MultiVector<Observer*> UsedObservers;
  };
}

#endif /* EventHandler_h */
