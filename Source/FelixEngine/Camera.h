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
    Camera(Scene *scene): Component("Camera", scene) {}
    virtual ~Camera() {}
    
    virtual bool setToXml(const XMLTree::Node *node)
    {
      bool success = Component::setToXml(node);
      if (success)
      {
        addChildren(*node);
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
      mView = dynamic_cast<View*>(getChildByType("View"));
      if (!mView)
      {
        mView = new View(mScene);
        addChild(mView);
      }
    }
    
    void setProjection()
    {
      mProjection = dynamic_cast<Projection*>(getChildByType("Projection"));
      if (!mProjection)
      {
        mProjection = new Projection(mScene);
        addChild(mProjection);
      }
    }
    
    void setRenderSlots()
    {
      mRenderSlots = dynamic_cast<RenderSlots*>(getChildByType("RenderSlots"));
      if (!mRenderSlots)
      {
        mRenderSlots = new RenderSlots(mScene);
        addChild(mRenderSlots);
      }
    }
    
  protected:
    View *mView;
    Projection *mProjection;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Camera_h */
