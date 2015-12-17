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
#include "View.h"
#include "Projection.h"
#include "RenderSlots.h"


namespace fx
{
  class Camera: public Component
  {
  public:
    Camera(Scene *scene): Component("Camera", scene), mView(0), mProjection(0), mRenderSlots(0) {}
    virtual ~Camera() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      addChildren(node);
      mView = getChildByType<View>("View");
      mProjection = getChildByType<Projection>("Projection");
      mRenderSlots = getChildByType<RenderSlots>("RenderSlots");
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
