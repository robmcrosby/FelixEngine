//
//  Meshes.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Meshes_h
#define Meshes_h

#include "Component.h"

namespace fx
{
  /**
   *
   */
  class Meshes: public Component
  {
  public:
    Meshes(Object *obj);
    virtual ~Meshes();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
  };
}

#endif /* Meshes_h */
