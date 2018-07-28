//
//  ARCamera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 8/5/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "ARCamera.h"
#include "TrackerSystem.h"


using namespace fx;
using namespace std;


ARCameraBuilder ARCamera::arCameraBuilder = ARCameraBuilder();

ARCamera::ARCamera() {
  
}

ARCamera::~ARCamera() {
  
}

void ARCamera::update(float td) {
  TrackerSystem *tracker = &TrackerSystem::getInstance();
  if (tracker != nullptr) {
    setView(tracker->getCameraView());
    setProjection(tracker->getCameraProjection());
  }
  Camera::update(td);
}
