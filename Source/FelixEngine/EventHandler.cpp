//
//  EventHandler.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "EventHandler.h"
#include "TaskGroup.h"


using namespace fx;
using namespace std;


Pool<EventHandler::Observer> EventHandler::ObserverPool;
List<EventHandler::Observer*> EventHandler::UsedObservers;


EventHandler::EventHandler()
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


void EventHandler::notify(const fx::Event &event)
{
  if (event.dispatchType() == DISPATCH_SERIAL)
    notifySerial(event);
  else if (event.dispatchType() == DISPATCH_SINGLE)
    notifySingle(event);
  else if (event.dispatchType() == DISPATCH_MULTIPLE)
    notifyMultiple(event, nullptr);
  else  if (event.dispatchType() == DISPATCH_BLOCK)
  {
    TaskGroup group;
    notifyMultiple(event, &group);
    group.waitOnTasks();
  }
}

void EventHandler::notifySerial(Event event)
{
  event.setSender(this);
  
  mObservers.lock();
  for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
  {
    EventHandler *handler = (*itr)->handler();
    if (handler)
    {
      event.setTarget(handler);
      handler->handle(event);
      ++itr;
    }
    else
      itr = mObservers.remove(itr);
  }
  mObservers.unlock();
}

void EventHandler::notifySingle(const Event &event)
{
  TaskingSystem *system = TaskingSystem::Instance();
  if (system)
  {
    TaskDelegate delegate = TaskDelegate::Create<EventHandler, &EventHandler::dispatchSingle>(this);
    Event *copy = event.copy();
    copy->setSender(this);
    system->dispatch(delegate, (void*)copy);
  }
}

void EventHandler::notifyMultiple(const Event &event, TaskGroup *group)
{
  TaskingSystem *system = TaskingSystem::Instance();
  if (system)
  {
    TaskDelegate delegate = TaskDelegate::Create<EventHandler, &EventHandler::dispatchMultiple>(this);
    
    mObservers.lock();
    for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
    {
      EventHandler *handler = (*itr)->handler();
      if (handler)
      {
        Event *copy = event.copy();
        copy->setSender(this);
        copy->setTarget(handler);
        system->dispatch(delegate, (void*)copy, group);
        ++itr;
      }
      else
        itr = mObservers.remove(itr);
    }
    mObservers.unlock();
  }
}


void EventHandler::dispatchSingle(void *ptr)
{
  Event &event = *static_cast<Event*>(ptr);
  event.setSender(this);
  
  mObservers.lock();
  for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
  {
    EventHandler *handler = (*itr)->handler();
    if (handler)
    {
      event.setTarget(handler);
      handler->handle(event);
      ++itr;
    }
    else
      itr = mObservers.remove(itr);
  }
  mObservers.unlock();
  
  Event::EventPool().freeItem(&event);
}

void EventHandler::dispatchMultiple(void *ptr)
{
  Event &event = *static_cast<Event*>(ptr);
  if (event.target())
    event.target()->handle(event);
  Event::EventPool().freeItem(&event);
}








//EventHandler::EventHandler(): mHandleEventDelegate(HandleEventDelegate::Create<EventHandler, &EventHandler::handleInline>(this))
//{
//  setEventFlags(EVENT_ALL);
//
//  mObserver = ObserverPool.newItem();
//  mObserver->setHandler(this);
//}

//EventHandler::~EventHandler()
//{
//  mObserver->clearHandler();
//  UsedObservers.pushBackSafe(mObserver);
//}
//
//void EventHandler::notify(const Event &event, bool block)
//{
//  if (block)
//  {
//    TaskGroup group;
//    notify(event, &group);
//    group.waitOnTasks();
//  }
//  else
//    notify(event, nullptr);
//}
//
//void EventHandler::notify(const Event &event, TaskGroup *group)
//{
//  mObservers.lock();
//  for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
//  {
//    EventHandler *handler = (*itr)->handler();
//    if (handler)
//    {
//      event.notify(handler, this, group);
//      ++itr;
//    }
//    else
//      itr = mObservers.remove(itr);
//  }
//  
//  mObservers.unlock();
//}
