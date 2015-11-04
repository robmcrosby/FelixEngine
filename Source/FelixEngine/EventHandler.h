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
        mObservers.pushBackSafe(handler->mObserver);
    }
    void removeHandler(const EventHandler *handler)
    {
      if (handler)
        mObservers.removeSafe(handler->mObserver);
    }
    
    virtual void handle(const Event &event) {}
    
    void notify(const Event &event);
    
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
    
  private:
    void notifySerial(Event event);
    void notifySingle(const Event &event);
    void notifyMultiple(const Event &event, TaskGroup *group);
    
    void dispatchSingle(void *ptr);
    void dispatchMultiple(void *ptr);
    
    mutable SDL_atomic_t mEventFlags;
    Observer *mObserver;
    List<Observer*> mObservers;
  };
}

#endif /* EventHandler_h */
