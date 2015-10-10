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

namespace fx
{
  /**
   *
   */
  class RenderSlots: public Component
  {
  public:
    RenderSlots();
    virtual ~RenderSlots();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
  };
}

#endif /* RenderSlots_h */
