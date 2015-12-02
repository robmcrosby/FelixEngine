//
//  Model.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include "Object.h"
#include "Transform.h"
#include "RenderSlots.h"


namespace fx
{
  /**
   *
   */
  class Model: public Object
  {
  public:
    Model(Scene *scene): Object("Model", scene), mTransform(0), mRenderSlots(0) {}
    virtual ~Model() {}
    
    virtual bool setToXml(const XMLTree::Node *node)
    {
      bool success = Object::setToXml(node);
      if (success)
      {
        setTransform();
        setRenderSlots();
      }
      return success;
    }
    
    Transform* transform() const {return mTransform;}
    RenderSlots* renderSlots() const {return mRenderSlots;}
    
  private:
    void setTransform()
    {
      mTransform = dynamic_cast<Transform*>(getComponentByType("Transform"));
      if (!mTransform)
        mTransform = new Transform(this);
    }
    
    void setRenderSlots()
    {
      mRenderSlots = dynamic_cast<RenderSlots*>(getComponentByType("RenderSlots"));
      if (!mRenderSlots)
        mRenderSlots = new RenderSlots(this);
    }
    
  protected:
    Transform *mTransform;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Model_h */
