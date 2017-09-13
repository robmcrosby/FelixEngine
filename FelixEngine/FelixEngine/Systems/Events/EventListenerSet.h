//
//  EventListenerSet.h
//  FelixEngine
//
//  Created by Robert Crosby on 9/12/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef EventListenerSet_h
#define EventListenerSet_h

#include "EventListener.h"

namespace fx {
  
  /** Contains and manages Event Listeners */
  class EventListenerSet {
  private:
    EventListeners _listeners;
    
  public:
    EventListenerSet() {}
    ~EventListenerSet() {clear();}
    
    void clear() {
      for (EventListeners::iterator itr = _listeners.begin(); itr != _listeners.end(); ++itr)
        (*itr)->release();
      _listeners.clear();
    }
    
    void clean() {
      EventListeners::iterator itr = _listeners.begin();
      while (itr != _listeners.end()) {
        EventListener *listener = *itr;
        if (listener->isEmpty()) {
          itr = _listeners.erase(itr);
          listener->release();
        }
      }
    }
    
    void notify(const Event &event) {
      EventListeners::iterator itr = _listeners.begin();
      while (itr != _listeners.end()) {
        EventListener *listener = *itr;
        if (listener->isEmpty()) {
          itr = _listeners.erase(itr);
          listener->release();
        }
        else {
          listener->handle(event);
          ++itr;
        }
      }
    }
    
    void add(EventListener *listener) {
      for (EventListeners::iterator itr = _listeners.begin(); itr != _listeners.end(); ++itr) {
        if (*itr == listener)
          return;
      }
      listener->retain();
      _listeners.push_back(listener);
    }
    
    void remove(EventListener *listener) {
      for (EventListeners::iterator itr = _listeners.begin(); itr != _listeners.end(); ++itr) {
        EventListener *listner = *itr;
        if (listner == listener) {
          _listeners.erase(itr);
          listner->release();
        }
      }
    }
  };
}


#endif /* EventListenerSet_h */
