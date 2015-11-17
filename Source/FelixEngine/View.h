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
    View(Object *obj);
    virtual ~View();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    void updateMatrix();
    
  private:
    void update(void*);
    
    mat4 mMatrix;
    RenderSlots *mRenderSlots;
  };
}

#endif /* View_h */
