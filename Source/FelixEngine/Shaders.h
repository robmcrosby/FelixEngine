//
//  Shaders.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Shaders_h
#define Shaders_h

#include "Component.h"

namespace fx
{
  /**
   *
   */
  class Shaders: public Component
  {
  public:
    Shaders(Object *obj);
    virtual ~Shaders();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
  };
}

#endif /* Shaders_h */
