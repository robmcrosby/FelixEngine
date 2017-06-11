//
//  Model.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include "Component.h"
#include "RenderSlots.h"
#include "Transform.h"


namespace fx
{
  /**
   *
   */
  class Model: public Component
  {
  public:
    Model(Scene *scene): Component(scene), mTransform(0), mRenderSlots(0) {}
    virtual ~Model() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      addChildren(node);
      mTransform = getChild<Transform>();
      mRenderSlots = getChild<RenderSlots>();
    }
    
    Transform* transform() const {return mTransform;}
    RenderSlots* renderSlots() const {return mRenderSlots;}
    
  protected:
    Transform *mTransform;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Model_h */
