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
    vec3 rotationRate() const
    {
      mMotionSem.wait();
      vec3 ret = mRotationRate;
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
    
  protected:
    vec3 mAcceleration;
    vec3 mRotationRate;
    vec3 mGravity;
    mutable Semaphore mMotionSem;
    
  private:
    static MotionSystem *sInstance;
  };
}


#endif /* MotionSystem_h */
