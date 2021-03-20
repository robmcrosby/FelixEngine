//
//  MotionCamera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 1/3/21.
//  Copyright © 2021 Robert Crosby. All rights reserved.
//

#include "MotionCamera.h"
#include "MotionSystem.h"


using namespace fx;
using namespace std;


MotionCameraBuilder MotionCamera::motionCameraBuilder = MotionCameraBuilder();

MotionCamera::MotionCamera() {
  _motionLocation = _transform->addTranslation();
}

MotionCamera::~MotionCamera() {
  _motionLocation = nullptr;
}

void MotionCamera::update(float td) {
  MotionSystem *motionSystem = &MotionSystem::getInstance();
  
  quat rotation = (motionSystem->getOrientation().inverse() * quat::RotX(M_PI/2.0f)).inverse();
  setRotation(rotation);
  
  *_motionLocation = rotation * vec3(0.0f, 0.0f, 1.0f) * 20.0f;
  
  Camera::update(td);
}
