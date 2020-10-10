//
//  EventListenerSet.h
//  FelixEngine
//
//  Created by Robert Crosby on 9/12/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef EventListenerSet_h
#define EventListenerSet_h

#include <FelixEngine/Event.h>

namespace fx {
  
  /** Contains and manages Event Listeners */
  class EventListenerSet {
  private:
    EventListeners _listeners;
    
  public:
    EventListenerSet() {}
    ~EventListenerSet() {clear();}
    
    void clear() {_listeners.clear();}
    
    void clean() {
      EventListeners::iterator itr = _listeners.begin();
      while (itr != _listeners.end()) {
        if ((*itr)->isEmpty())
          itr = _listeners.erase(itr);
      }
    }
    
    void notify(const Event &event) {
      EventListeners::iterator itr = _listeners.begin();
      while (itr != _listeners.end()) {
        if ((*itr)->isEmpty()) {
          itr = _listeners.erase(itr);
        }
        else {
          (*itr)->handle(event);
          ++itr;
        }
      }
    }
    
    void add(EventListener listener) {
      for (EventListeners::iterator itr = _listeners.begin(); itr != _listeners.end(); ++itr) {
        if (*itr == listener)
          return;
      }
      _listeners.push_back(listener);
    }
    
    void remove(EventListener listener) {
      EventListeners::iterator itr = _listeners.begin();
      while (itr != _listeners.end()) {
        if (*itr == listener)
          itr = _listeners.erase(itr);
        else
          ++itr;
      }
    }
  };
}


#endif /* EventListenerSet_h */
