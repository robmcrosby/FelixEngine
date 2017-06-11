//
//  Condition.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/8/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Condition_h
#define Condition_h

#include <SDL2/SDL.h>

namespace fx
{
  class Condition
  {
  public:
    Condition(): mEnabled(true)
    {
      mMutex = SDL_CreateMutex();
      mCondition = SDL_CreateCond();
    }
    ~Condition()
    {
      SDL_DestroyCond(mCondition);
      SDL_DestroyMutex(mMutex);
    }
    void wait()
    {
      SDL_LockMutex(mMutex);
      if (mEnabled)
        SDL_CondWait(mCondition, mMutex);
      SDL_UnlockMutex(mMutex);
    }
    void broadcast(bool enable = true)
    {
      SDL_LockMutex(mMutex);
      mEnabled = enable;
      SDL_CondBroadcast(mCondition);
      SDL_UnlockMutex(mMutex);
    }
    void signal(bool enable = true)
    {
      SDL_LockMutex(mMutex);
      mEnabled = enable;
      SDL_CondSignal(mCondition);
      SDL_UnlockMutex(mMutex);
    }
    void setEnabled(bool enable) {mEnabled = enable;}
    bool enabled() const {return mEnabled;}
    
  private:
    bool mEnabled;
    SDL_mutex *mMutex;
    SDL_cond *mCondition;
  };
}

#endif /* Condition_h */
