//
//  ARPlaneDetect.cpp
//  ARPlaneDetect
//
//  Created by Robert Crosby on 8/9/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "ARPlaneDetect.h"
#include <FelixEngine/Camera.h>
#include <FelixEngine/Model.h>
#include <FelixEngine/Material.h>


using namespace std;

ARPlaneDetect::ARPlaneDetect() {
  _model = nullptr;
}

ARPlaneDetect::~ARPlaneDetect() {
  
}

void ARPlaneDetect::initalize() {
  _scene.loadXMLFile("Scene.xml");
  _model = _scene.getModel("Model");
}

void ARPlaneDetect::update() {
  _scene.update();
}

void ARPlaneDetect::render() {
  _scene.render();
}

void ARPlaneDetect::trackedPlaneAdded(fx::TrackedPlane plane) {
  if (_model != nullptr) {
    _model->setPosition(plane.center);
    _model->update();
  }
  cout << "Plane Added: " << plane.center << endl;
}

void ARPlaneDetect::trackedPlaneUpdated(fx::TrackedPlane plane) {
  if (_model != nullptr) {
    _model->setPosition(plane.center);
    _model->update();
  }
  cout << "Plane Updated: " << plane.center << endl;
}

void ARPlaneDetect::trackedPlaneRemoved(fx::TrackedPlane plane) {
  cout << "Plane Removed" << endl;
}
