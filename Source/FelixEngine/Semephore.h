//
//  Semephore.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Semephore_h
#define Semephore_h

#include <SDL2/SDL.h>

namespace fx
{
  class Semephore
  {
  public:
    Semephore(size_t size = 0) {setup(size);}
    Semephore(const Semephore &other) {setup(other.mSize);}
    ~Semephore()
    {
      SDL_DestroyMutex(mLockMutex);
      SDL_DestroySemaphore(mSemaphore);
      delete [] mIndexQueue;
    }
    
    Semephore& operator=(const Semephore &other) {return *this;}
    
    void wait() {SDL_SemWait(mSemaphore);}
    void post() {SDL_SemPost(mSemaphore);}
    
    size_t waitForIndex()
    {
      wait();
      int i = SDL_AtomicAdd(&mDequeueIndex, 1) % mSize;
      return mIndexQueue[i].index;
    }
    
    void postIndex(size_t index)
    {
      int i = SDL_AtomicAdd(&mEnqueueIndex, 1) % mSize;
      mIndexQueue[i].index = index;
      post();
    }
    
    void lock()
    {
      SDL_LockMutex(mLockMutex);
      for (unsigned int i = 0; i < mSize; ++i)
        wait();
    }
    void unlock()
    {
      for (unsigned int i = 0; i < mSize; ++i)
        post();
      SDL_UnlockMutex(mLockMutex);
    }
    
    size_t size() const {return mSize;}
    
  private:
    struct Index
    {
      size_t index;
      char buffer[SDL_CACHELINE_SIZE-sizeof(size_t)];
    };
    
  private:
    void setup(size_t size)
    {
      // Setup the Semaphore and Lock Mutex
      mSize = size ? size : SDL_GetCPUCount();
      mSemaphore = SDL_CreateSemaphore((unsigned int)mSize);
      mLockMutex = SDL_CreateMutex();
      
      // Setup the Index Queue
      mIndexQueue = new Index[mSize];
      for (int i = 0; i < mSize; ++i)
        mIndexQueue[i].index = i;
      SDL_AtomicSet(&mDequeueIndex, 0);
      SDL_AtomicSet(&mEnqueueIndex, 0);
    }
    
    size_t     mSize;
    SDL_sem   *mSemaphore;
    SDL_mutex *mLockMutex;
    
    Index *mIndexQueue;
    SDL_atomic_t mDequeueIndex;
    SDL_atomic_t mEnqueueIndex;
  };
}

#endif /* Semephore_h */
