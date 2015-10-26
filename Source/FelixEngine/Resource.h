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
  class Resource
  {
  public:
    Resource(): mLoaded(0), mLoading(0), mUnloading(0), mRemoved(0) {SDL_AtomicSet(&mCount, 0);}
    virtual ~Resource() {}
    
    bool setToXml(const XMLTree::Node *node) {return node && setToXml(*node);}
    virtual bool setToXml(const XMLTree::Node &node) = 0;
    
    void retain() {SDL_AtomicIncRef(&mCount);}
    void release()
    {
      if (SDL_AtomicDecRef(&mCount))
        mRemoved = true;
    }
    
    bool isLoaded() const {return mLoaded;}
    bool isLoading() const {return mLoading;}
    bool isUnloading() const {return mUnloading;}
    bool isRemoved() const {return mRemoved;}
    
    void setToLoad() {mLoading = true;}
    void setToUnload() {mUnloading = true;}
    
    void setLoaded()
    {
      mLoaded = true;
      mLoading = false;
    }
    void setUnloaded()
    {
      mLoaded = false;
      mUnloading = false;
    }
    
  protected:
    void setNotLoading() {mLoading = false;}
    void setNotUnloading() {mUnloading = false;}
    
  private:
    bool mLoaded, mLoading, mUnloading, mRemoved;
    SDL_atomic_t mCount;
  };
}


#endif /* Resource_h */
