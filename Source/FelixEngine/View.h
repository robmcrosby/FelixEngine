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
    View(Object *obj): Component("View", obj), mRenderSlots(0)
    {
      mUpdateDelegate = UpdateDelegate::Create<View, &View::update>(this);
    }
    virtual ~View() {}
    
    virtual bool init()
    {
      mRenderSlots = static_cast<RenderSlots*>(mObject->getComponentByType("RenderSlots"));
      return mRenderSlots ? Component::init() : false;
    }
    
    mat4 matrix4x4() const {return mMatrix;}
    void setMatrix(const mat4 &matrix) {mMatrix = matrix;}
    
    RenderSlots* renderSlots() {return mRenderSlots;}
    
  protected:
    void update(void*)
    {
      if (mRenderSlots)
        mRenderSlots->localUniforms()["View"] = mMatrix;
    }
    
  private:
    mat4 mMatrix;
    RenderSlots *mRenderSlots;
  };
}

#endif /* View_h */
