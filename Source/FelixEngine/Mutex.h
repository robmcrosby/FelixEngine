//
//  Mutex.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Mutex_h
#define Mutex_h

#include <SDL2/SDL.h>

namespace fx
{
  class Mutex
  {
  public:
    Mutex() {mMutex = SDL_CreateMutex();}
    ~Mutex() {SDL_DestroyMutex(mMutex);}
    
    void lock() {SDL_LockMutex(mMutex);}
    void unlock() {SDL_UnlockMutex(mMutex);}
    
    bool tryLock() {return !SDL_TryLockMutex(mMutex);}
    
  private:
    SDL_mutex *mMutex;
  };
}

#endif /* Mutex_h */
