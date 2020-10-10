//
//  EventHandler.h
//  FelixEngine
//
//  Created by Robert Crosby on 9/3/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef EventHandler_h
#define EventHandler_h

#include <list>
#include <FelixEngine/EventListenerSet.h>


namespace fx {
  
  /** Concrete class that implements iEventHandler */
  class EventHandler: public iEventHandler {
  private:
    EventListener _listener;
    
  protected:
    unsigned int _eventFlags;
    
  public:
    EventHandler(): _eventFlags(EVENT_ALL), _listener(new Listener(this)) {}
    EventHandler(const EventHandler &other): _listener(new Listener(this)) {
      *this = other;
    }
    virtual ~EventHandler() {_listener->setEmpty();}
    
    virtual EventHandler& operator=(const EventHandler &other) {
      _eventFlags = other._eventFlags;
      return *this;
    }
    
    virtual unsigned int eventFlags() const {return _eventFlags;}
    virtual void handle(const Event &event) {}
    virtual EventListener listener() const {return _listener;}
  };
}

#endif /* EventHandler_hpp */
