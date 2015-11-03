//
//  EventHandler.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "EventHandler.h"
#include "TaskGroup.h"
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
