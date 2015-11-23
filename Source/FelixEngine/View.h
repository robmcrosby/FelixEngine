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
#include "Object.h"
#include "RenderSlots.h"
#include "Matrix.h"

namespace fx
{
  class RenderSlots;
  
  /**
   *
   */
  class View: public Component
  {
  public:
    View(Object *obj): Component("View", obj), mRenderSlots(0), mActive(true), mLock(0)
    {
      mUpdateDelegate = UpdateDelegate::Create<View, &View::update>(this);
    }
    virtual ~View() {}
    
    virtual bool init()
    {
      mRenderSlots = static_cast<RenderSlots*>(mObject->getComponentByType("RenderSlots"));
      return mRenderSlots ? Component::init() : false;
    }
    
    void setActive(bool active) {mActive = active;}
    bool active() const {return mActive;}
    
    mat4 matrix4x4() const
    {
      SDL_AtomicLock(&mLock);
      mat4 ret = mMatrix;
      SDL_AtomicUnlock(&mLock);
      return ret;
    }
    void setMatrix(const mat4 &matrix)
    {
      SDL_AtomicLock(&mLock);
      mMatrix = matrix;
      SDL_AtomicUnlock(&mLock);
    }
    
    RenderSlots* renderSlots() {return mRenderSlots;}
    
  protected:
    void update(void*)
    {
      if (mActive && mRenderSlots)
      {
        SDL_AtomicLock(&mLock);
        mRenderSlots->localUniforms()["View"] = mMatrix;
        SDL_AtomicUnlock(&mLock);
      }
    }
    
  protected:
    bool mActive;
    
  private:
    mat4 mMatrix;
    mutable SDL_SpinLock mLock;
    RenderSlots *mRenderSlots;
  };
}

#endif /* View_h */
