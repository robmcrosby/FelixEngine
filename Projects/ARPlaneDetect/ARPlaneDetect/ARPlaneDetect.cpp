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
  _plane = nullptr;
}

ARPlaneDetect::~ARPlaneDetect() {
  
}

void ARPlaneDetect::initalize() {
  _scene.loadXMLFile("Scene.xml");
  
  _model = _scene.get<fx::Model>("Model");
  _model->setHidden();
  
  _plane = _scene.get<fx::Model>("Plane");
  _plane->setHidden();
  
  _renderScheme.push_back("MainPass");
}

void ARPlaneDetect::update(float td) {
  _scene.update(td);
  
  fx::RenderPass::renderPasses(_renderScheme);
  fx::RenderPass::resetPasses();
}

void ARPlaneDetect::trackedPlaneAdded(fx::TrackedPlane plane) {
  if (_model != nullptr) {
    fx::mat4 transform = fx::mat4::Trans3d(fx::vec3(0.0f, 0.05f, 0.0f)) * plane.transform;
    _model->setPosition(transform * plane.center);
    _model->setHidden(false);
    
    _plane->setPosition(plane.transform * plane.center);
    _plane->setScale(fx::vec3(plane.extent.x, 1.0, plane.extent.y));
    //_plane->setHidden(false);
  }
}

void ARPlaneDetect::trackedPlaneUpdated(fx::TrackedPlane plane) {
  if (_model != nullptr) {
    fx::mat4 transform = fx::mat4::Trans3d(fx::vec3(0.0f, 0.05f, 0.0f)) * plane.transform;
    _model->setPosition(transform * plane.center);
    
    _plane->setPosition(plane.transform * plane.center);
    _plane->setScale(fx::vec3(plane.extent.x, 1.0, plane.extent.y));
  }
}

void ARPlaneDetect::trackedPlaneRemoved(fx::TrackedPlane plane) {
  cout << "Plane Removed" << endl;
}
