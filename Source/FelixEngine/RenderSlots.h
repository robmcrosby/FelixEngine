//
//  RenderSlots.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef RenderSlots_h
#define RenderSlots_h

#include "Component.h"
#include "UniformMap.h"

namespace fx
{
  class GraphicSystem;
  
  /**
   *
   */
  class RenderSlots: public Component
  {
  public:
    RenderSlots(Object *obj);
    virtual ~RenderSlots();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
  private:
    GraphicSystem *mGraphicSystem;
    UniformMap mUniforms;
  };
}

#endif /* RenderSlots_h */
