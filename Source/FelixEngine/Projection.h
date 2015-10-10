//
//  Projection.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Projection_h
#define Projection_h

#include "Component.h"

namespace fx
{
  /**
   *
   */
  class Projection: public Component
  {
  public:
    Projection();
    virtual ~Projection();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
  };
}

#endif /* Projection_h */
