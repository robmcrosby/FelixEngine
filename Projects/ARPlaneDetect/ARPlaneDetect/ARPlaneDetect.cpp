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

ARPlaneDetect::ARPlaneDetect(): _trackerSystem(0) {
  
}

ARPlaneDetect::~ARPlaneDetect() {
  
}

void ARPlaneDetect::initalize() {
  _scene.loadXMLFile("Scene.xml");
  
  // Define a Material
  fx::MaterialPtr material = _scene.get<fx::Material>("Material");
  material->loadShader("basic_vertex", "basic_fragment");
  material->enableDepthTesting();
  material->setAmbiant(fx::vec3(0.7, 0.7, 0.7), 0.2);
  material->setDiffuse(fx::vec3(0.7, 0.7, 0.7), 0.8);
  material->setSpecular(fx::vec3(1.0, 1.0, 1.0), 0.6);
  material->setSpecularHardness(20.0);
  
  _model = _scene.get<fx::Model>("Model");
  _model->loadMesh("bunny.mesh");
  _model->setMaterial(material);
  _model->setFaceCulling(fx::CULL_BACK);
  _model->setToRenderPass("MainPass");
  _model->setInstances(0);
  
}

void ARPlaneDetect::update(float td) {
  if (_trackerSystem) {
    const fx::TrackedPlanes &planes = _trackerSystem->trackedPlanes();
    _model->setInstances((unsigned int)planes.size());
    int index = 0;
    for (auto plane : planes) {
      _model->setScale(0.01f, index);
      _model->setRotation(fx::quat::RotX(M_PI/2.0f) * fx::quat::RotZ(M_PI/2.0f), index);
      _model->setLocation(plane.transform * vec3(0.0f, 0.0f, 0.0f) + plane.center, index++);
    }
  }
  _scene.update(td);
}

void ARPlaneDetect::setTrackerSystem(fx::TrackerSystem *tracker) {
  _trackerSystem = tracker;
  tracker->setDelegate(this);
}

void ARPlaneDetect::trackedPlaneAdded(fx::TrackedPlane plane) {
  //cout << "Tracked Plane Added" << endl;
}

void ARPlaneDetect::trackedPlaneUpdated(fx::TrackedPlane plane) {
  //cout << "Tracked Plane Updated" << endl;
}

void ARPlaneDetect::trackedPlaneRemoved(fx::TrackedPlane plane) {
  //cout << "Tracked Plane Removed" << endl;
}
