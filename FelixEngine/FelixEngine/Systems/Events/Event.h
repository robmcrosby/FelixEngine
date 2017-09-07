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
  class EventHandler;
  
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
  };
  
  struct Event {
    unsigned int catagory, type;
    EventHandler *sender;
    Variant message;
    
    Event(): catagory(0), type(0), sender(0) {}
    Event(USER_TYPE t): catagory(EVENT_USER), type(t) {}
    Event(INPUT_TYPE t): catagory(EVENT_INPUT), type(t) {}
  };
}


#endif /* Event_h */
