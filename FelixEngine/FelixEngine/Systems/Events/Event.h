//
//  Event.h
//  FelixEngine
//
//  Created by Robert Crosby on 8/28/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Event_h
#define Event_h

#include "Variant.h"

namespace fx {
  class iEventHandler;
  class iEventSubject;
  class EventListener;
  
  enum EVENT_CATAGORY {
    EVENT_ALL   = 0xffff,
    EVENT_USER  = 0x0001,
    EVENT_INPUT = 0x0002,
  };
  
  enum USER_TYPE {
    USER_BUTTON_DOWN,
    USER_BUTTON_UP,
  };
  
  enum INPUT_TYPE {
    INPUT_TOUCH_DOWN,
    INPUT_TOUCH_MOVE,
    INPUT_TOUCH_UP,
    INPUT_TOUCH_CANCEL,
  };
  
  enum TOUCH_TYPE {
    TOUCH_FINGER,
    TOUCH_STYLUS,
  };
  
  struct Touch {
    Touch(): number(0), force(1.0f), type(TOUCH_FINGER) {}
    
    int number;
    float force;
    vec2 location;
    vec2 pevious;
    vec2 azimuth;
    TOUCH_TYPE type;
  };
  typedef std::vector<Touch> Touches;
  
  struct Event {
    unsigned int catagory, type;
    iEventSubject *subject;
    Variant message;
    
    Event(): catagory(0), type(0), subject(0) {}
    Event(USER_TYPE t): catagory(EVENT_USER), type(t) {}
    Event(INPUT_TYPE t, Touches touches): catagory(EVENT_INPUT), type(t) {
      message = touches;
    }
  };
  
  /** Interface for handling Events */
  struct iEventHandler {
    virtual ~iEventHandler() {}
    virtual unsigned int eventFlags() const = 0;
    virtual void handle(const Event &event) = 0;
    virtual EventListener* listener() const = 0;
  };
  
  /** Interface for Event Subject */
  struct iEventSubject {
    virtual ~iEventSubject() {}
    virtual void notifyEventHandlers(const Event &event) = 0;
    virtual void addEventHandler(iEventHandler *handler) = 0;
    virtual void removeEventHandler(iEventHandler *handler) = 0;
    virtual void clearEventHandlers() = 0;
  };
}


#endif /* Event_h */
