//
//  EventHandler.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/28/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef EventHandler_h
#define EventHandler_h

#include "Pool.h"
#include "TaskGroup.h"
#include "List.h"


namespace fx
{
  class Event;
  
  class EventHandler
  {
  public:
    typedef Delegate<void, const Event&> HandleEventDelegate;
    
  private:
    friend Event;
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
    
  public:
    EventHandler();
    virtual ~EventHandler();
    
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
    
    void notify(const Event &event, bool block = false);
    virtual void handle(const Event &event) {}
    
    static void Cleanup()
    {
      UsedObservers.lock();
      for (List<Observer*>::Iterator itr = UsedObservers.begin(); itr != UsedObservers.end(); ++itr)
        ObserverPool.freeItem(*itr);
      UsedObservers.clear();
      UsedObservers.unlock();
    }
    
    void setEventFlags(int flags) {SDL_AtomicSet(&mEventFlags, flags);}
    int eventFlags() const {return SDL_AtomicGet(&mEventFlags);}
    
    HandleEventDelegate getHandleEventDelegate() const {return mHandleEventDelegate;}
    
  protected:
    HandleEventDelegate mHandleEventDelegate;
    
  private:
    void notify(const Event &event, TaskGroup *group);
    void handleInline(const Event &event) {handle(event);}
    
    mutable SDL_atomic_t mEventFlags;
    Observer *mObserver;
    List<Observer*> mObservers;
    
  private:
    static Pool<Observer> ObserverPool;
    static List<Observer*> UsedObservers;
  };
}

#endif /* EventHandler_h */
