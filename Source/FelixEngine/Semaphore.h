//
//  Semaphore.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Semaphore_h
#define Semaphore_h

#include <SDL2/SDL.h>

namespace fx
{
  class Semaphore
  {
  public:
    Semaphore(unsigned int size = 0) {setup(size);}
    Semaphore(const Semaphore &other) {setup(other.mSize);}
    ~Semaphore()
    {
      SDL_DestroyMutex(mLockMutex);
      SDL_DestroySemaphore(mSemaphore);
    }
    
    Semaphore& operator=(const Semaphore &other) {return *this;}
    
    void wait() {SDL_SemWait(mSemaphore);}
    void post() {SDL_SemPost(mSemaphore);}
    bool tryWait() {return !SDL_SemTryWait(mSemaphore);}
    
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
    
    unsigned int size() const {return mSize;}
    unsigned int value() const {return SDL_SemValue(mSemaphore);}
    
  private:
    void setup(unsigned int size)
    {
      mSize = size ? size : SDL_GetCPUCount();
      mSemaphore = SDL_CreateSemaphore(mSize);
      mLockMutex = SDL_CreateMutex();
    }
    
    unsigned int mSize;
    SDL_sem   *mSemaphore;
    SDL_mutex *mLockMutex;
    
    
  };
  
  class IndexedSemaphore: public Semaphore
  {
  public:
    IndexedSemaphore(unsigned int size = 0): Semaphore(size) {setup();}
    IndexedSemaphore(const Semaphore &other): Semaphore(other) {setup();}
    ~IndexedSemaphore() {delete [] mIndexQueue;}
    
    int waitForIndex()
    {
      wait();
      int i = SDL_AtomicAdd(&mDequeueIndex, 1) % size();
      return mIndexQueue[i].index;
    }
    int tryWaitForIndex()
    {
      if (tryWait())
      {
        int i = SDL_AtomicAdd(&mDequeueIndex, 1) % size();
        return mIndexQueue[i].index;
      }
      return -1;
    }
    void postIndex(int index)
    {
      int i = SDL_AtomicAdd(&mEnqueueIndex, 1) % size();
      mIndexQueue[i].index = index;
      post();
    }
    
  private:
    struct Index
    {
      int index;
      char buffer[SDL_CACHELINE_SIZE-sizeof(int)];
    };
    
  private:
    void setup()
    {
      mIndexQueue = new Index[size()];
      for (int i = 0; i < size(); ++i)
        mIndexQueue[i].index = i;
      SDL_AtomicSet(&mDequeueIndex, 0);
      SDL_AtomicSet(&mEnqueueIndex, 0);
    }
    
    Index *mIndexQueue;
    SDL_atomic_t mDequeueIndex;
    SDL_atomic_t mEnqueueIndex;
  };
}

#endif /* Semaphore_h */
