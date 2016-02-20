//
//  Resource.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/16/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Resource_h
#define Resource_h

#include <SDL2/SDL_atomic.h>
#include "XMLTree.h"

namespace fx
{
  class Resource;
  
  class Resource
  {
  public:
    Resource(): mLoaded(0), mRemoved(0) {SDL_AtomicSet(&mCount, 0);}
    virtual ~Resource() {}
    
    void retain() {SDL_AtomicIncRef(&mCount);}
    void release()
    {
      if (SDL_AtomicDecRef(&mCount))
        mRemoved = true;
    }
    
    bool loaded() const {return mLoaded;}
    bool removed() const {return mRemoved;}
    
    void setLoaded(bool loaded = true) {mLoaded = loaded;}
    void setUnloaded() {setLoaded(false);}
    
  public:
    static void Replace(Resource **dst, Resource *src)
    {
      if (*dst != src)
      {
        if (src != nullptr)
          src->retain();
        if (*dst != nullptr)
          (*dst)->release();
        *dst = src;
      }
    }
    
    template <typename T>
    static void Replace(T **dst, T *src) {Replace((Resource**)dst, (Resource*)src);}
    
    
  private:
    bool mLoaded;
    bool mRemoved;
    SDL_atomic_t mCount;
  };
}


#endif /* Resource_h */
