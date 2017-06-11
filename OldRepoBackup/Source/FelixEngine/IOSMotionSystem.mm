//
//  IOSMotionSystem.mm
//  FelixEngine
//
//  Created by Robert Crosby on 11/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "IOSMotionSystem.h"


fx::IOSMotionSystem* fx::IOSMotionSystem::Instance = nullptr;


#if __IPHONEOS__
#import <CoreMotion/CoreMotion.h>


namespace fx
{
  struct MotionInfo
  {
    MotionInfo() {mManager = nil; mAttitude = nil;}
    ~MotionInfo() {mManager = nil; mAttitude = nil;}
    
    CMMotionManager *mManager;
    CMAttitude *mAttitude;
  };
}


using namespace fx;
using namespace std;


IOSMotionSystem::IOSMotionSystem()
{
  mInfo = new MotionInfo();
  Instance = this;
  mUpdateFrequecy = DEFAULT_FREQUENCY;
}

IOSMotionSystem::~IOSMotionSystem()
{
  if (mInfo)
  {
    if (mInfo->mManager != nil)
    {
      [mInfo->mManager stopAccelerometerUpdates];
      [mInfo->mManager stopGyroUpdates];
    }
    delete mInfo;
  }
}

bool IOSMotionSystem::setToXml(const XMLTree::Node *node)
{
  if (node)
  {
    if (node->hasAttribute("frequency"))
      setUpdateFrequency(node->attributeAsFloat("frequency"));
  }
  return true;
}

void IOSMotionSystem::setUpdateFrequency(float frequency)
{
  if (mUpdateFrequecy != frequency)
  {
    mUpdateFrequecy = frequency;
    if (mInfo && mInfo->mManager != nil)
    {
      NSTimeInterval interval = 1.0/mUpdateFrequecy;
      if (mInfo->mManager.deviceMotionActive)
        [mInfo->mManager setDeviceMotionUpdateInterval:interval];
      if (mInfo->mManager.accelerometerActive)
        [mInfo->mManager setAccelerometerUpdateInterval:interval];
      if (mInfo->mManager.gyroActive)
        [mInfo->mManager setGyroUpdateInterval:interval];
    }
  }
}

void IOSMotionSystem::handleMotionData(vec3 gravity, vec3 acceleration, vec3 rotation, quat orientation)
{
  mMotionSem.lock();
  mGravity = gravity;
  mAcceleration = acceleration;
  mRotation = rotation;
  mMotionSem.unlock();
  
  // Dispatch a motion event
  Event event(EVENT_MOTION);
  event.motionData().gravity = gravity;
  event.motionData().acceleration = acceleration;
  event.motionData().rotation = rotation;
  event.motionData().orientation = orientation;
  notify(event);
}


bool IOSMotionSystem::init()
{
  bool success = false;
  NSTimeInterval interval = 1.0/mUpdateFrequecy;
  mInfo->mManager = [[CMMotionManager alloc] init];
  
  if (mInfo->mManager.deviceMotionAvailable)
  {
    //cout << "Device Motion avalible" << endl;
    [mInfo->mManager setDeviceMotionUpdateInterval:interval];
    [mInfo->mManager startDeviceMotionUpdatesToQueue:[NSOperationQueue currentQueue]
                                         withHandler:^(CMDeviceMotion *motion, NSError *error) {
      vec3 gravity(motion.gravity.x, motion.gravity.y, motion.gravity.z);
      vec3 accelration(motion.userAcceleration.x, motion.userAcceleration.y, motion.userAcceleration.z);
      vec3 rotationRate(motion.rotationRate.x, motion.rotationRate.y, motion.rotationRate.z);
      
      CMQuaternion q = motion.attitude.quaternion;
      quat rotation(q.x, q.y, q.z, q.w);
                                           
      Instance->handleMotionData(gravity, accelration, rotationRate, rotation);
     }];
    
    success = true;
  }
  else
    cerr << "Device Motion Missing" << endl;
  
  return success;
}

#else

using namespace fx;
using namespace std;


IOSMotionSystem::IOSMotionSystem()
{
}

IOSMotionSystem::~IOSMotionSystem()
{
}

void IOSMotionSystem::handleMotionData(vec3 gravity, vec3 acceleration, vec3 rotationRate, quat rotation)
{
}

bool IOSMotionSystem::setToXml(const XMLTree::Node *node)
{
  return false;
}

bool IOSMotionSystem::init()
{
  return false;
}

#endif
