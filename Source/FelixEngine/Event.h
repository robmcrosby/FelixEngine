//
//  Event.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/28/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Event_h
#define Event_h

#include "EventHandler.h"
#include "Platform.h"
#include "Task.h"
#include "Pool.h"


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
  
  class Event: public Task
  {
  public:
    Event(EVENT_TYPE type = EVENT_NONE): mType(type), mSender(0), mTarget(0), mInPool(0)
    {
      mDelegate = TaskDelegate::Create<Event, &Event::execute>(this);
      setTimeStamp();
    }
    Event(const Event &other): mInPool(0)
    {
      mDelegate = TaskDelegate::Create<Event, &Event::execute>(this);
      *this = other;
    }
    
    Event& operator=(const Event &other)
    {
      mType = other.mType;
      mTimeStamp = other.mTimeStamp;
      mSender = other.mSender;
      mTarget = other.mTarget;
      return *this;
    }
    
    bool operator==(EVENT_TYPE type) const {return mType == type;}
    
    void setType(EVENT_TYPE type) {mType = type;}
    EVENT_TYPE type() const {return mType;}
    
    void setTimeStamp() {mTimeStamp = Platform::GetTimeStamp();}
    void setTimeStamp(unsigned int ts) {mTimeStamp = ts;}
    unsigned int timeStamp() const {return mTimeStamp;}
    
    void setSender(EventHandler *sender) {mSender = sender;}
    EventHandler* sender() const {return mSender;}
    
    void setTarget(EventHandler *target) {mTarget = target;}
    EventHandler* target() const {return mTarget;}
    
    void notify(EventHandler *target, EventHandler *sender, TaskGroup *group) const
    {
      Event *event = EventPool().newItem();
      *event = *this;
      event->mInPool = true;
      event->mTarget = target;
      event->mSender = sender;
      event->dispatch(group);
    }
    
  private:
    void execute(void*)
    {
      if (mTarget)
        mTarget->getHandleEventDelegate()(*this);
      if (mInPool)
        EventPool().freeItem(this);
    }
    
    EVENT_TYPE    mType;
    unsigned int  mTimeStamp;
    
    bool mInPool;
    EventHandler *mSender;
    EventHandler *mTarget;
    
  private:
    static Pool<Event>& EventPool()
    {
      static Pool<Event> pool;
      return pool;
    }
  };
}


#endif /* Event_h */
