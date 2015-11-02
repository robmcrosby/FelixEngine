//
//  Event.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/28/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Event_h
#define Event_h

#include "Platform.h"


namespace fx
{
  class EventHandler;
  
  /**
   *
   */
  enum EVENT_TYPE
  {
    EVENT_NONE    = 0x00,
    EVENT_RENDER  = 0x01,
    EVENT_UPDATE  = 0x02,
    EVENT_INPUT   = 0x04,
    EVENT_OTHER   = 0xF0,
    EVENT_ALL     = 0xFF,
  };
  
  class Event
  {
  public:
    Event(EVENT_TYPE type = EVENT_NONE, EventHandler *sender = 0): mSender(sender) {setTimeStamp();}
    Event(const Event &other) {*this = other;}
    ~Event() {}
    
    Event& operator=(const Event &other)
    {
      mType      = other.mType;
      mTimeStamp = other.mTimeStamp;
      mSender    = other.mSender;
      return *this;
    }
    
    void setType(EVENT_TYPE type) {mType = type;}
    EVENT_TYPE type() const {return mType;}
    
    void setTimeStamp() {mTimeStamp = Platform::GetTimeStamp();}
    void setTimeStamp(unsigned int ts) {mTimeStamp = ts;}
    unsigned int timeStamp() const {return mTimeStamp;}
    
    void setSender(EventHandler *sender) {mSender = sender;}
    EventHandler* sender() const {return mSender;}
    
    
    
  private:
    EVENT_TYPE    mType;
    unsigned int  mTimeStamp;
    EventHandler *mSender;
  };
}


#endif /* Event_h */
