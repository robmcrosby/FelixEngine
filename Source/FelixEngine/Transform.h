//
//  Transform.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/9/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Transform_hpp
#define Transform_hpp

#include "Component.h"

namespace fx
{
  /**
   *
   */
  class Transform: public Component
  {
  public:
    Transform();
    virtual ~Transform();
    
    virtual bool setToXml(const XMLTree::Node *node);
  };
}

#endif /* Transform_hpp */
