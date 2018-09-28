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
#import <FelixEngine/Graphics.h>

#define GRID_SCALE 20.0f

using namespace std;

ARPlaneDetect::ARPlaneDetect(): _trackerSystem(0) {
  
}

ARPlaneDetect::~ARPlaneDetect() {
  
}

void ARPlaneDetect::initalize() {
  _scene.loadXMLFile("Scene.xml");
  
  fx::TextureBufferPtr dotTexture = _scene.getTextureBuffer("DotTexture");
  dotTexture->loadFile("dot.png");
  
  fx::SamplerState dotTextureSampler;
  dotTextureSampler.setMinFilter(fx::FILTER_LINEAR);
  dotTextureSampler.setMagFilter(fx::FILTER_LINEAR);
  dotTextureSampler.setSCoord(fx::COORD_REPEAT);
  dotTextureSampler.setTCoord(fx::COORD_REPEAT);
  dotTextureSampler.setRCoord(fx::COORD_REPEAT);
  
  // Define a Material
  fx::MaterialPtr material = _scene.get<fx::Material>("Material");
  material->loadShader("v_texture", "f_texture_shadeless");
  material->enableDepthTesting();
  material->setAmbiant(fx::vec3(1.0, 1.0, 1.0), 0.6);
  material->addTexture(dotTexture, dotTextureSampler);
  material->blendState().enableDefaultBlending();
  
  fx::VertexMeshPtr planeMesh = fx::MeshBuilder::createPlane();
  _planes = _scene.get<fx::Model>("Planes");
  _planes->setMesh(planeMesh);
  _planes->setMaterial(material);
  _planes->setToRenderPass("MainPass");
  //_planes->setFaceCulling(fx::CULL_FRONT);
  _planes->setInstances(0);
  
  _bunny = _scene.get<fx::Model>("Bunny");
  _bunny->setHidden();
}

void ARPlaneDetect::update(float td) {
  if (_trackerSystem) {
    const fx::ARPlanes &planes = _trackerSystem->trackedPlanes();
    _planes->setInstances((unsigned int)planes.size());
    int index = 0;
    for (auto plane : planes) {
      _planes->setModelTransform(plane.transform, index);
      _planes->setRotation(fx::quat::RotX(M_PI/2.0f), index);
      _planes->setScale(fx::vec3(plane.extent.x/2.0, plane.extent.y/2.0, 1.0f), index);
      _planes->setLocation(plane.center, index);
      _planes->setTextureTransform(mat4::Scale(fx::vec3(GRID_SCALE*plane.extent.x, GRID_SCALE*plane.extent.y, 1.0f)), index);
      
//      _planes->setRotation(plane.rotation * fx::quat::RotX(M_PI/2.0f), index);
//      _planes->setScale(fx::vec3(plane.extent.x/2.0, plane.extent.y/2.0, 1.0f), index);
//      _planes->setLocation(plane.position + plane.center, index);
//      _planes->setTextureTransform(mat4::Scale(fx::vec3(GRID_SCALE*plane.extent.x, GRID_SCALE*plane.extent.y, 1.0f)), index);
      
      index++;
    }
  }
  _scene.update(td);
}

void ARPlaneDetect::handle(const fx::Event &event) {
  if (event.catagory == fx::EVENT_INPUT) {
    if (event.type == fx::INPUT_TOUCH_DOWN)
      handleTouchDown(event);
  }
}

void ARPlaneDetect::handleTouchDown(const fx::Event &event) {
  // Disable Plane Detection
  if (_trackerSystem->planeDetectionEnabled())
    _trackerSystem->disablePlaneDetection();
  
  if (event.message.size() > 0) {
    const Touch &touch = (const Touch&)event.message[0];
    ARHitResults results = _trackerSystem->hitTest(touch);
    for (auto &result : results) {
      if (result.onPlane) {
        _bunny->setModelTransform(result.world);
        _bunny->setHidden(false);
        break;
      }
    }
  }
}

void ARPlaneDetect::setTrackerSystem(fx::TrackerSystem *tracker) {
  _trackerSystem = tracker;
  tracker->setDelegate(this);
}

void ARPlaneDetect::trackedPlaneAdded(fx::ARPlane plane) {
  //cout << "Tracked Plane Added" << endl;
}

void ARPlaneDetect::trackedPlaneUpdated(fx::ARPlane plane) {
  //cout << "Tracked Plane Updated" << endl;
}

void ARPlaneDetect::trackedPlaneRemoved(fx::ARPlane plane) {
  //cout << "Tracked Plane Removed" << endl;
}
