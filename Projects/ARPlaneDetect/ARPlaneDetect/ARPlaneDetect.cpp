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
    fx::mat4 transform = fx::mat4::Trans3d(fx::vec3(0.0f, 0.04f, 0.0f)) * plane.transform;
    _model->setPosition(transform * plane.center);
    _model->update();
  }
}

void ARPlaneDetect::trackedPlaneUpdated(fx::TrackedPlane plane) {
  if (_model != nullptr) {
    fx::mat4 transform = fx::mat4::Trans3d(fx::vec3(0.0f, 0.04f, 0.0f)) * plane.transform;
    _model->setPosition(transform * plane.center);
    _model->update();
  }
}

void ARPlaneDetect::trackedPlaneRemoved(fx::TrackedPlane plane) {
  cout << "Plane Removed" << endl;
}
