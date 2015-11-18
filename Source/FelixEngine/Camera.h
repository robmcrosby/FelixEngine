//
//  Camera.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include "Object.h"
#include "View.h"
#include "Projection.h"
#include "RenderSlots.h"


namespace fx
{
  class Camera: public Object
  {
  public:
    Camera(Scene *scene): Object("Camera", scene) {}
    virtual ~Camera() {}
    
    virtual bool setToXml(const XMLTree::Node *node)
    {
      bool success = Object::setToXml(node);
      if (success)
      {
        setView();
        setProjection();
        setRenderSlots();
      }
      return success;
    }
    
    View* view() const {return mView;}
    Projection* projection() const {return mProjection;}
    RenderSlots* renderSlots() const {return mRenderSlots;}
    
  private:
    void setView()
    {
      mView = dynamic_cast<View*>(getComponentByType("View"));
      if (!mView)
      {
        mView = new View(this);
        addComponent(mView);
      }
    }
    
    void setProjection()
    {
      mProjection = dynamic_cast<Projection*>(getComponentByType("Projection"));
      if (!mProjection)
      {
        mProjection = new Projection(this);
        addComponent(mProjection);
      }
    }
    
    void setRenderSlots()
    {
      mRenderSlots = dynamic_cast<RenderSlots*>(getComponentByType("RenderSlots"));
      if (!mRenderSlots)
      {
        mRenderSlots = new RenderSlots(this);
        addComponent(mRenderSlots);
      }
    }
    
  protected:
    View *mView;
    Projection *mProjection;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Camera_h */
