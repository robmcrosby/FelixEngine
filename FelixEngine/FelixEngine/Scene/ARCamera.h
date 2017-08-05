//
//  ARCamera.h
//  FelixEngine
//
//  Created by Robert Crosby on 8/5/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef ARCamera_h
#define ARCamera_h

#include "Camera.h"

namespace fx {
  class ARCamera: public Camera {
  public:
    ARCamera(Scene *scene);
    virtual ~ARCamera();
    
    virtual void update();
  };
}

#endif /* ARCamera_h */
