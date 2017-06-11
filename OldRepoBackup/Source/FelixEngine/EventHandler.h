//
//  EventHandler.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/28/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef EventHandler_h
#define EventHandler_h

#include "Event.h"
#include "Pool.h"
#include "List.h"

#include "TaskGroup.h"

namespace fx
{
  class Event;
  class TaskGroup;
  
  class EventHandler
  {
  private:
    class Observer
    {
    public:
      Observer(): mPtr(0) {}
      void setHandler(EventHandler *handler) {SDL_AtomicSetPtr(&mPtr, (void*)handler);}
      void clearHandler() {SDL_AtomicSetPtr(&mPtr, nullptr);}
      EventHandler* handler() {return static_cast<EventHandler*>(SDL_AtomicGetPtr(&mPtr));}
    private:
      void *mPtr;
    };
    static Pool<Observer> ObserverPool;
    static List<Observer*> UsedObservers;
    
  public:
    EventHandler();
    virtual ~EventHandler();
    
    void addHandler(const EventHandler *handler)
    {
      if (handler && handler != this)
        mObservers.pushBack(handler->mObserver);
    }
    void removeHandler(const EventHandler *handler)
    {
      if (handler)
        mObservers.remove(handler->mObserver);
    }
    
    virtual void handle(const Event &event) {}
    
    void notify(const Event &event) {notify(event, event.dispatchType());}
    void notify(const Event &event, DISPATCH_TYPE dispatch)
    {
      if (dispatch == DISPATCH_SERIAL)
        notifySerial(event);
      else if (dispatch == DISPATCH_SINGLE)
        notifySingle(event);
      else if (dispatch == DISPATCH_MULTIPLE)
        notifyMultiple(event, nullptr);
      else  if (dispatch == DISPATCH_BLOCK)
      {
        TaskGroup group;
        notifyMultiple(event, &group);
        group.waitOnTasks();
      }
    }
    
    static void Cleanup()
    {
      for (List<Observer*>::Iterator itr = UsedObservers.begin(); itr != UsedObservers.end(); ++itr)
        ObserverPool.freeItem(*itr);
      UsedObservers.clear();
    }
    
    void setEventFlags(int flags) {SDL_AtomicSet(&mEventFlags, flags);}
    int eventFlags() const {return SDL_AtomicGet(&mEventFlags);}
    
  private:
    void notifySerial(Event event)
    {
      event.setSender(this);
      
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
    }
    
    void notifySingle(const Event &event);
    void notifyMultiple(const Event &event, TaskGroup *group)
    {
      //TaskingSystem *system = TaskingSystem::Instance();
      //if (system)
      //{
      //  TaskDelegate delegate = TaskDelegate::Create<EventHandler, &EventHandler::dispatchMultiple>(this);
      std::cout << "notifyMultiple" << std::endl;
        for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
        {
          EventHandler *handler = (*itr)->handler();
          if (handler)
          {
            if (handler->eventFlags() & event.type())
            {
              notifyMultipleDispatch(event, group, handler);
//              Event *copy = event.copy();
//              copy->setSender(this);
//              copy->setTarget(handler);
//              system->dispatch(delegate, (void*)copy, group);
            }
            ++itr;
          }
          else
            itr = mObservers.remove(itr);
        }
      //}
    }
    void notifyMultipleDispatch(const Event &event, TaskGroup *group, EventHandler *handler);
    
    void dispatchSingle(void *ptr)
    {
      Event &event = *static_cast<Event*>(ptr);
      event.setSender(this);
      
      for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
      {
        EventHandler *handler = (*itr)->handler();
        if (handler)
        {
          if (handler->eventFlags() & event.type())
          {
            event.setTarget(handler);
            handler->handle(event);
          }
          ++itr;
        }
        else
          itr = mObservers.remove(itr);
      }
      
      Event::EventPool().freeItem(&event);
    }
    
    void dispatchMultiple(void *ptr)
    {
      Event &event = *static_cast<Event*>(ptr);
      if (event.target())
        event.target()->handle(event);
      Event::EventPool().freeItem(&event);
    }
    
    mutable SDL_atomic_t mEventFlags;
    Observer *mObserver;
    List<Observer*> mObservers;
  };
}

#endif /* EventHandler_h */
