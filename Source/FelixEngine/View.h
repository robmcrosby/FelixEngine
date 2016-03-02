//
//  View.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/17/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef View_h
#define View_h

#include "Component.h"
#include "RenderSlots.h"
#include "Matrix.h"

namespace fx
{
  /**
   *
   */
  class View: public Component
  {
  public:
    View(Scene *scene): Component(scene), mLock(0)
    {
    }
    virtual ~View() {}
    
    mat4 getView() const
    {
      lock();
      mat4 ret = mViewMatrix;
      unlock();
      return ret;
    }
    
    void lock() const {SDL_AtomicLock(&mLock);}
    void unlock() const {SDL_AtomicUnlock(&mLock);}
    
  protected:
    mat4 mViewMatrix;
    
  private:
    mutable SDL_SpinLock mLock;
  };
}

#endif /* View_h */
