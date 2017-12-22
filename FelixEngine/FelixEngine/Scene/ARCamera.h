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
#include "GraphicTask.h"

namespace fx {
  DEFINE_OBJ_BUILDER(ARCamera)
  
  class ARCamera: public Camera {
  protected:
    static ARCameraBuilder arCameraBuilder;
    
  public:
    ARCamera();
    virtual ~ARCamera();
    
    virtual void update(float td);
    virtual void applyToTask(GraphicTask &task);
  };
}

#endif /* ARCamera_h */
