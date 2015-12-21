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
    View(Scene *scene): Component(scene), mRenderSlots(0), mActive(true), mLock(0)
    {
    }
    virtual ~View() {}
    
    virtual bool init()
    {
      mRenderSlots = getSibling<RenderSlots>();
      return mRenderSlots ? Component::init() : false;
    }
    
    void setActive(bool active) {mActive = active;}
    bool active() const {return mActive;}
    
    mat4 matrix4x4() const
    {
      lock();
      mat4 ret = mMatrix;
      unlock();
      return ret;
    }
    void setMatrix(const mat4 &matrix)
    {
      lock();
      mMatrix = matrix;
      unlock();
    }
    
    RenderSlots* renderSlots()
    {
      lock();
      RenderSlots *ret = mRenderSlots;
      unlock();
      return ret;
    }
    
    void lock() const {SDL_AtomicLock(&mLock);}
    void unlock() const {SDL_AtomicUnlock(&mLock);}
    
  protected:
    virtual void update()
    {
      lock();
      if (mActive && mRenderSlots)
      {
        for (RenderSlots::iterator itr = mRenderSlots->begin(); itr != mRenderSlots->end(); ++itr)
          (*itr)->uniforms().set("View", mMatrix);
      }
      unlock();
      Component::update();
    }
    
  protected:
    mat4 mMatrix;
    bool mActive;
    
  private:
    mutable SDL_SpinLock mLock;
    RenderSlots *mRenderSlots;
  };
}

#endif /* View_h */
