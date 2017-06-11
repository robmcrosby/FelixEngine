//
//  EventHandler.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "EventHandler.h"
#include "TaskingSystem.h"


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
  UsedObservers.pushBack(mObserver);
}

//void EventHandler::notify(const fx::Event &event, DISPATCH_TYPE dispatch)
//{
//  if (dispatch == DISPATCH_SERIAL)
//    notifySerial(event);
//  else if (dispatch == DISPATCH_SINGLE)
//    notifySingle(event);
//  else if (dispatch == DISPATCH_MULTIPLE)
//    notifyMultiple(event, nullptr);
//  else  if (dispatch == DISPATCH_BLOCK)
//  {
//    TaskGroup group;
//    notifyMultiple(event, &group);
//    group.waitOnTasks();
//  }
//}

//void EventHandler::notifySerial(Event event)
//{
//  event.setSender(this);
//  
//  for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
//  {
//    EventHandler *handler = (*itr)->handler();
//    if (handler)
//    {
//      event.setTarget(handler);
//      handler->handle(event);
//      ++itr;
//    }
//    else
//      itr = mObservers.remove(itr);
//  }
//}

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

//void EventHandler::notifyMultiple(const Event &event, TaskGroup *group)
//{
//  TaskingSystem *system = TaskingSystem::Instance();
//  if (system)
//  {
//    TaskDelegate delegate = TaskDelegate::Create<EventHandler, &EventHandler::dispatchMultiple>(this);
//    
//    for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
//    {
//      EventHandler *handler = (*itr)->handler();
//      if (handler)
//      {
//        if (handler->eventFlags() & event.type())
//        {
//          Event *copy = event.copy();
//          copy->setSender(this);
//          copy->setTarget(handler);
//          system->dispatch(delegate, (void*)copy, group);
//        }
//        ++itr;
//      }
//      else
//        itr = mObservers.remove(itr);
//    }
//  }
//}

void EventHandler::notifyMultipleDispatch(const Event &event, TaskGroup *group, EventHandler *handler)
{
  TaskingSystem *system = TaskingSystem::Instance();
  if (system)
  {
    TaskDelegate delegate = TaskDelegate::Create<EventHandler, &EventHandler::dispatchMultiple>(this);
    
    Event *copy = event.copy();
    copy->setSender(this);
    copy->setTarget(handler);
    system->dispatch(delegate, (void*)copy, group);
  }
}

//
//void EventHandler::dispatchSingle(void *ptr)
//{
//  Event &event = *static_cast<Event*>(ptr);
//  event.setSender(this);
//  
//  for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
//  {
//    EventHandler *handler = (*itr)->handler();
//    if (handler)
//    {
//      if (handler->eventFlags() & event.type())
//      {
//        event.setTarget(handler);
//        handler->handle(event);
//      }
//      ++itr;
//    }
//    else
//      itr = mObservers.remove(itr);
//  }
//  
//  Event::EventPool().freeItem(&event);
//}

//void EventHandler::dispatchMultiple(void *ptr)
//{
//  Event &event = *static_cast<Event*>(ptr);
//  if (event.target())
//    event.target()->handle(event);
//  Event::EventPool().freeItem(&event);
//}
