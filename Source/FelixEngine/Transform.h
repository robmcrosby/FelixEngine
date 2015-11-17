//
//  Transform.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/9/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Transform_h
#define Transform_h

#include "Component.h"

namespace fx
{
  class RenderSlots;
  
  /**
   *
   */
  class Transform: public Component
  {
  public:
    Transform(Object *obj);
    virtual ~Transform();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    virtual void update();
    
  private:
    RenderSlots *mRenderSlots;
  };
}

#endif /* Transform_h */
