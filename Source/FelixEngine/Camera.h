//
//  Camera.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include "Object.h"


namespace fx
{
  class Camera: public Object
  {
  public:
    Camera(Scene *scene): Object("Camera", scene) {}
    virtual ~Camera() {}
  };
}

#endif /* Camera_h */
