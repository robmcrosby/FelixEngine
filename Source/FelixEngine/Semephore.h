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
    Semephore(unsigned int count = 0) {setup(count);}
    Semephore(const Semephore &other) {setup(other.mCount);}
    ~Semephore()
    {
      SDL_DestroyMutex(mLockMutex);
      SDL_DestroySemaphore(mSemaphore);
    }
    
    Semephore& operator=(const Semephore &other) {return *this;}
    
    void wait() {SDL_SemWait(mSemaphore);}
    void post() {SDL_SemPost(mSemaphore);}
    
    void lock()
    {
      SDL_LockMutex(mLockMutex);
      for (unsigned int i = 0; i < mCount; ++i)
        wait();
    }
    void unlock()
    {
      for (unsigned int i = 0; i < mCount; ++i)
        post();
      SDL_UnlockMutex(mLockMutex);
    }
    
    unsigned int count() const {return mCount;}
    
  private:
    void setup(unsigned int count)
    {
      mCount = count ? count : (unsigned int)SDL_GetCPUCount();
      mSemaphore = SDL_CreateSemaphore(mCount);
      mLockMutex = SDL_CreateMutex();
    }
    
    unsigned int mCount;
    SDL_sem *mSemaphore;
    SDL_mutex *mLockMutex;
  };
}

#endif /* Semephore_h */
