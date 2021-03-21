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

MotionCamera::MotionCamera(): _distance(10.0f) {
  _motionLocation = _transform->addTranslation();
}

MotionCamera::~MotionCamera() {
  _motionLocation = nullptr;
}

bool MotionCamera::load(const XMLTree::Node &node) {
  bool success = Camera::load(node);
  
  // Set the distance
  if (node.hasAttribute("distance"))
    setDistance(node.attributeAsFloat("distance"));
  
  return success;
}

void MotionCamera::update(float td) {
  MotionSystem *motionSystem = &MotionSystem::getInstance();
  
  quat rotation = (motionSystem->getOrientation().inverse() * quat::RotX(M_PI/2.0f)).inverse();
  setRotation(rotation);
  
  *_motionLocation = rotation * vec3(0.0f, 0.0f, 1.0f) * _distance;
  
  Camera::update(td);
}
