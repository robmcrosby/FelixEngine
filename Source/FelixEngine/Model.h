//
//  Model.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include "Object.h"


namespace fx
{
  /**
   *
   */
  class Model: public Object
  {
  public:
    Model(Scene *scene): Object("Model", scene) {}
    virtual ~Model() {}
  };
}

#endif /* Model_h */
