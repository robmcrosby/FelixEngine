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
#include "Matrix.h"

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
    
    void updateMatrices();
    
  private:
    void update(void*);
    
    mat4 mModelMatrix;
    mat4 mRotationMatrix;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Transform_h */
