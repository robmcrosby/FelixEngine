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
    
  protected:
    View *mView;
    Projection *mProjection;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Camera_h */
