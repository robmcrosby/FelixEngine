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
    
  private:
    TranslateOpPtr _motionLocation;
    float _distance;
    
  public:
    MotionCamera();
    virtual ~MotionCamera();
    
    virtual bool load(const XMLTree::Node &node);
    virtual void update(float td);
    
    void setDistance(float distance) {_distance = distance;}
    float distance() const {return _distance;}
  };
}

#endif /* MotionCamera_h */
