//
//  iOSMotionSystem.m
//  FelixEngine
//
//  Created by Robert Crosby on 11/25/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "iOSMotionSystem.h"
#include <CoreMotion/CMMotionManager.h>

using namespace std;
using namespace fx;


MotionSystem* MotionSystem::instance = nullptr;


iOSMotionSystem::iOSMotionSystem() {
  MotionSystem::instance = this;
  _motionManager = nil;
}

iOSMotionSystem::~iOSMotionSystem() {
  stop();
}

bool iOSMotionSystem::initalize(double interval) {
  _motionManager = [[CMMotionManager alloc] init];
  _motionManager.deviceMotionUpdateInterval = 1.0 / 60.0;
  [_motionManager startDeviceMotionUpdatesUsingReferenceFrame: CMAttitudeReferenceFrameXArbitraryCorrectedZVertical];
  return true;
}

void iOSMotionSystem::stop() {
  if (_motionManager != nil) {
    [_motionManager stopDeviceMotionUpdates];
    _motionManager = nil;
  }
}

quat iOSMotionSystem::getOrientation() {
  CMDeviceMotion *deviceMotion = _motionManager.deviceMotion;
  if (deviceMotion != nil) {
    CMQuaternion q = deviceMotion.attitude.quaternion;
    return quat(q.x, q.y, q.z, q.w);
  }
  return quat();
}
