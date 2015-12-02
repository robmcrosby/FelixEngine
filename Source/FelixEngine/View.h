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
    void update(void*)
    {
      lock();
      if (mActive && mRenderSlots)
      {
        for (RenderSlots::iterator itr = mRenderSlots->begin(); itr != mRenderSlots->end(); ++itr)
          (*itr)->uniforms().set("View", mMatrix);
      }
      unlock();
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
