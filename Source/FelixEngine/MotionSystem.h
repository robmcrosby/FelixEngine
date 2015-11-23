//
//  MotionSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef MotionSystem_h
#define MotionSystem_h

#include "System.h"
#include "Semaphore.h"


namespace fx
{
  class FelixEngine;
  
  class MotionSystem: public System
  {
  public:
    static MotionSystem* Instance() {return sInstance;}
    
    MotionSystem(): System(SYSTEM_MOTION) {}
    virtual ~MotionSystem() {}
    
    vec3 acceleration() const
    {
      mMotionSem.wait();
      vec3 ret = mAcceleration;
      mMotionSem.post();
      return ret;
    }
    vec3 rotation() const
    {
      mMotionSem.wait();
      vec3 ret = mRotation;
      mMotionSem.post();
      return ret;
    }
    vec3 gravity() const
    {
      mMotionSem.wait();
      vec3 ret = mGravity;
      mMotionSem.post();
      return ret;
    }
    quat orientation() const
    {
      mMotionSem.wait();
      quat ret = mOrientation;
      mMotionSem.post();
      return ret;
    }
    
  protected:
    vec3 mAcceleration;
    vec3 mRotation;
    vec3 mGravity;
    quat mOrientation;
    mutable Semaphore mMotionSem;
    
  private:
    static MotionSystem *sInstance;
  };
}


#endif /* MotionSystem_h */
