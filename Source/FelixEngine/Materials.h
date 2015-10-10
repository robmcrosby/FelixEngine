//
//  Materials.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Materials_h
#define Materials_h

#include "Component.h"

namespace fx
{
  /**
   *
   */
  class Materials: public Component
  {
  public:
    Materials();
    virtual ~Materials();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
  };
}

#endif /* Materials_h */
