//
//  EventSubject.h
//  FelixEngine
//
//  Created by Robert Crosby on 9/13/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef EventSubject_h
#define EventSubject_h

#include "EventHandler.h"


namespace fx {
  
  /** Concrete class that implements iEventHandler */
  class EventSubject: public EventHandler, public iEventSubject {
  private:
    EventListenerSet _listeners;
    
  public:
    EventSubject() {}
    virtual ~EventSubject() {clearEventHandlers();}
    
    virtual void notifyEventHandlers(const Event &event) {_listeners.notify(event);}
    virtual void addEventHandler(iEventHandler *handler) {_listeners.add(handler->listener());}
    virtual void removeEventHandler(iEventHandler *handler) {_listeners.remove(handler->listener());}
    virtual void clearEventHandlers() {_listeners.clear();}
  };
}

#endif /* EventSubject_h */
