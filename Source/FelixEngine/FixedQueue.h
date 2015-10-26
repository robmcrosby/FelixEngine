//
//  FixedQueue.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef FixedQueue_h
#define FixedQueue_h

#include <SDL2/SDL.h>


namespace fx
{
  template<typename T>
  class FixedQueue
  {
  public:
    FixedQueue(size_t size): mSize((int)size), mFront(0), mBack(0), mLock(0)
    {
      mQueue = new T[mSize];
    }
    
    ~FixedQueue()
    {
      delete[] mQueue;
    }
    
    bool enqueue(const T &item)
    {
      bool success = false;
      SDL_AtomicLock(&mLock);
      
      if (mBack - mFront < mSize)
      {
        mQueue[mBack++ % mSize] = item;
        success = true;
      }
      
      SDL_AtomicUnlock(&mLock);
      return success;
    }
    
    bool dequeue(T &item)
    {
      bool success = false;
      SDL_AtomicLock(&mLock);
      
      if (mFront < mBack)
      {
        item = mQueue[mFront++ % mSize];
        success = true;
      }
      
      SDL_AtomicUnlock(&mLock);
      return success;
    }
    
    void clear()
    {
      SDL_AtomicLock(&mLock);
      
      mFront = mBack = 0;
      
      SDL_AtomicUnlock(&mLock);
    }
    
    size_t size()
    {
      SDL_AtomicLock(&mLock);
      
      int used = mBack - mFront;
      
      SDL_AtomicUnlock(&mLock);
      return (size_t)used;
    }
    
    size_t capacity() const {return (size_t)(mSize);}
    
  private:
    int mSize;
    int mFront;
    int mBack;
    SDL_SpinLock mLock;
    T *mQueue;
  };
}

#endif /* FixedQueue_h */
