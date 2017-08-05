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

DEFINE_CAMERA_BUILDER(ARCamera)


ARCamera::ARCamera(Scene *scene): Camera(scene) {
  cout << "Created ARCamera" << endl;
}

ARCamera::~ARCamera() {
  
}

void ARCamera::update() {
  TrackerSystem *tracker = &TrackerSystem::getInstance();
  if (tracker != nullptr) {
    setView(tracker->getCameraView());
    setProjection(tracker->getCameraProjection());
  }
}
