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
#include "TaskGroup.h"
#include "List.h"


namespace fx
{
  class EventHandler
  {
  public:
    typedef Delegate<void, const Event&> HandlerDelegate;
    
  private:
    class Observer
    {
    public:
      Observer(): mPtr(0) {}
      void setHandler(EventHandler *handler) {SDL_AtomicSetPtr(&mPtr, (void*)this);}
      void clearHandler() {SDL_AtomicSetPtr(&mPtr, nullptr);}
      EventHandler* handler() {return static_cast<EventHandler*>(SDL_AtomicGetPtr(&mPtr));}
      
    private:
      void *mPtr;
    };
    
  public:
    EventHandler()
    {
      mObserver = ObserverPool.newItem();
      mObserver->setHandler(this);
      setEventFlags(EVENT_ALL);
      mHandlerDelegate = HandlerDelegate::Create<EventHandler, &EventHandler::inlineHandler>(this);
    }
    virtual ~EventHandler()
    {
      mObserver->clearHandler();
      UsedObservers.pushBackSafe(mObserver);
    }
    
    void addHandler(const EventHandler *handler)
    {
      if (handler && handler != this)
        mObservers.pushBackSafe(handler->mObserver);
    }
    void removeHandler(const EventHandler *handler)
    {
      if (handler)
        mObservers.removeSafe(handler->mObserver);
    }
    
    virtual void handle(const Event &event) {}
    
    void dispatchSerial(const Event &event)
    {
      mObservers.lock();
      for (List<Observer*>::Iterator itr = mObservers.begin(); itr != mObservers.end();)
      {
        EventHandler *handler = (*itr)->handler();
        if (handler)
        {
          if (handler->eventFlags() & (int)event.type())
            handler->mHandlerDelegate(event);
          ++itr;
        }
        else
          itr = mObservers.remove(itr);
      }
      mObservers.unlock();
    }
    void dispatchParallel(const Event &event) {}
    void dispatchAndWait(const Event &event)
    {
      TaskGroup group;
    }
    
    static void ClearUnused()
    {
      UsedObservers.lock();
      for (List<Observer*>::Iterator itr = UsedObservers.begin(); itr != UsedObservers.end(); ++itr)
        ObserverPool.freeItem(*itr);
      UsedObservers.clear();
      UsedObservers.unlock();
    }
    
    void setEventFlags(int flags) {SDL_AtomicSet(&mEventFlags, flags);}
    int eventFlags() const {return SDL_AtomicGet(&mEventFlags);}
    
  protected:
    HandlerDelegate mHandlerDelegate;
    
  private:
    void inlineHandler(const Event &event) {handle(event);}
    
    mutable SDL_atomic_t mEventFlags;
    Observer *mObserver;
    List<Observer*> mObservers;
    
  private:
    static Pool<Observer> ObserverPool;
    static List<Observer*> UsedObservers;
  };
}

#endif /* EventHandler_h */
