//
//  Camera.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include "Component.h"
#include "RenderSlots.h"
#include "View.h"
#include "Projection.h"


namespace fx
{
  class Camera: public Component
  {
  public:
    Camera(Scene *scene): Component(scene), mView(0), mProjection(0), mRenderSlots(0) {}
    virtual ~Camera() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      addChildren(node);
      mView = getChild<View>();
      mProjection = getChild<Projection>();
      mRenderSlots = getChild<RenderSlots>();
    }
    
    View* view() const {return mView;}
    Projection* projection() const {return mProjection;}
    RenderSlots* renderSlots() const {return mRenderSlots;}
    
    virtual void update()
    {
      Component::update();
      if (mRenderSlots)
      {
        for (RenderSlot *slot : *mRenderSlots)
        {
          if (slot)
            applyToSlot(*slot);
        }
      }
    }
    
  protected:
    void applyToSlot(RenderSlot &slot)
    {
      // Apply the Projection
      if (mProjection)
      {
        mProjection->adjustSlotViewport(slot);
        vec2 size(slot.drawState().viewport.size);
        if (size == vec2())
        {
          Frame *frame = GetResource<Frame>(&slot.frame());
          size = frame ? vec2(frame->size()) : vec2(1.0f, 1.0f);
        }
        slot.setStruct("camera", "projection", mProjection->getProjection(size, slot.projectionFlags()));
      }
      else
      {
        slot.setStruct("camera", "projection", mat4());
      }
      
      // Apply the View
      if (mView)
      {
        slot.setStruct("camera", "view", mView->getView());
        slot.setStruct("camera", "position", vec4(mView->getPosition(), 1.0f));
      }
      else
      {
        slot.setStruct("camera", "view", mat4());
        slot.setStruct("camera", "position", vec4());
      }
    }
    
    View *mView;
    Projection *mProjection;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Camera_h */
