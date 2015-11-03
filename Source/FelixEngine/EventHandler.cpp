//
//  EventHandler.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "EventHandler.h"
#include "Event.h"


using namespace fx;
using namespace std;


Pool<EventHandler::Observer> EventHandler::ObserverPool;
List<EventHandler::Observer*> EventHandler::UsedObservers;



EventHandler::EventHandler(): mHandleEventDelegate(HandleEventDelegate::Create<EventHandler, &EventHandler::handleInline>(this))
{
  setEventFlags(EVENT_ALL);
  
  mObserver = ObserverPool.newItem();
  mObserver->setHandler(this);
}

EventHandler::~EventHandler()
{
  mObserver->clearHandler();
  UsedObservers.pushBackSafe(mObserver);
}

void EventHandler::notify(const Event &event, bool block)
{
  if (block)
  {
    TaskGroup group;
    notify(event, &group);
    group.waitOnTasks();
  }
  else
    notify(event, nullptr);
}

void EventHandler::notify(const Event &event, TaskGroup *group)
{
  mObservers.lock();
  for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
  {
    EventHandler *handler = (*itr)->handler();
    if (handler)
    {
      event.notify(handler, this, group);
      ++itr;
    }
    else
      itr = mObservers.remove(itr);
  }
  
  mObservers.unlock();
}


Event::Event(EVENT_TYPE type): mSender(0), mTarget(0), mInPool(0)
{
  mDelegate = TaskDelegate::Create<Event, &Event::execute>(this);
  setTimeStamp();
}

Event::Event(const Event &other): mInPool(0)
{
  mDelegate = TaskDelegate::Create<Event, &Event::execute>(this);
  *this = other;
}

Event& Event::operator=(const Event &other)
{
  mType = other.mType;
  mTimeStamp = other.mTimeStamp;
  mSender = other.mSender;
  mTarget = other.mTarget;
  return *this;
}
