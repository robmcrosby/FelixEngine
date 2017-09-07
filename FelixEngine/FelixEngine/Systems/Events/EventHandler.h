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
#include "Event.h"


namespace fx {
  class EventListener;
  typedef std::list<EventListener*> EventListeners;
  
  class EventHandler {
  private:
    EventListeners _listeners;
    EventListener *_listener;
    
  protected:
    unsigned int _eventFlags;
    
  public:
    EventHandler();
    EventHandler(const EventHandler &other);
    virtual ~EventHandler();
    
    virtual EventHandler& operator=(const EventHandler &handler);
    
    void notify(Event event);
    
    unsigned int eventFlags() const {return _eventFlags;}
    virtual void handle(const Event &event);
    
    void addListener(EventHandler *handler);
    void removeListener(EventHandler *handler);
    void clearListeners();
  };
}

#endif /* EventHandler_hpp */
