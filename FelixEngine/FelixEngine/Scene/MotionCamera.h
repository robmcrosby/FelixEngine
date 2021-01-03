//
//  MotionCamera.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 1/3/21.
//  Copyright © 2021 Robert Crosby. All rights reserved.
//

#ifndef MotionCamera_h
#define MotionCamera_h

#include "Camera.h"

namespace fx {
  DEFINE_OBJ_BUILDER(MotionCamera)
  
  class MotionCamera: public Camera {
  protected:
    static MotionCameraBuilder motionCameraBuilder;
    
  public:
    MotionCamera();
    virtual ~MotionCamera();
    
    virtual void update(float td);
  };
}

#endif /* MotionCamera_h */
