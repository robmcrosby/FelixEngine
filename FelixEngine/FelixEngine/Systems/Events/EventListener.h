//
//  EventListener.h
//  FelixEngine
//
//  Created by Robert Crosby on 9/11/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef EventListener_h
#define EventListener_h

#include "Event.h"

namespace fx {
  /** Listener Class that keeps an EventHandler refrence */
  class EventListener {
  private:
    iEventHandler *handler;
    int count;
    
  public:
    EventListener(iEventHandler *handler): handler(handler), count(1) {}
    void setEmpty() {
      handler = nullptr;
      release();
    }
    void retain() {++count;}
    void release() {
      if (--count <= 0)
        delete this;
    }
    bool isEmpty() const {return handler == nullptr;}
    void handle(const Event &event) {
      if (handler != nullptr && handler->eventFlags() & event.catagory)
        handler->handle(event);
    }
  };
  
  /** Container for Listeners */
  typedef std::list<EventListener*> EventListeners;
}


#endif /* EventListener_h */
