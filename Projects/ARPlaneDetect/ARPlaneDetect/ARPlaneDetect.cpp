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
#include <FelixEngine/MeshBuilder.h>

using namespace std;

ARPlaneDetect::ARPlaneDetect(): _trackerSystem(0) {
  
}

ARPlaneDetect::~ARPlaneDetect() {
  
}

void ARPlaneDetect::initalize() {
  _scene.loadXMLFile("Scene.xml");
  
  // Define a Material
  fx::MaterialPtr material = _scene.get<fx::Material>("Material");
  material->loadShader("v_shadeless", "f_shadeless");
  material->enableDepthTesting();
  material->setAmbiant(fx::vec3(0.7, 0.7, 0.7), 0.2);
  material->setDiffuse(fx::vec3(0.7, 0.7, 0.7), 0.8);
  material->setSpecular(fx::vec3(1.0, 1.0, 1.0), 0.6);
  material->setSpecularHardness(20.0);
  
  fx::VertexMeshPtr planeMesh = fx::MeshBuilder::createPlane();
  _planes = _scene.get<fx::Model>("Planes");
  _planes->setMesh(planeMesh);
  _planes->setMaterial(material);
  _planes->setToRenderPass("MainPass");
  _planes->setFaceCulling(fx::CULL_FRONT);
  _planes->setInstances(0);
}

void ARPlaneDetect::update(float td) {
  if (_trackerSystem) {
    const fx::TrackedPlanes &planes = _trackerSystem->trackedPlanes();
    _planes->setInstances((unsigned int)planes.size());
    int index = 0;
    for (auto plane : planes) {
      _planes->setRotation(fx::quat::RotX(M_PI/2.0f), index);
      _planes->setScale(fx::vec3(plane.extent.x/2.0, plane.extent.y/2.0, 1.0f), index);
      //_planes->setScale(0.1f, index);
      _planes->setLocation(plane.transform * vec3(0.0f, 0.0f, 0.0f) + plane.center, index);
      index++;
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
