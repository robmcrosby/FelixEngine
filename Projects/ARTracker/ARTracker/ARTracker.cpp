//
//  ARTracker.cpp
//  ARTracker
//
//  Created by Robert Crosby on 8/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "ARTracker.h"
#include <FelixEngine/Camera.h>
#include <FelixEngine/Model.h>
#include <FelixEngine/Material.h>


ARTracker::ARTracker() {
  printf("Create ARTracker\n");
}

ARTracker::~ARTracker() {
  
}

void ARTracker::initalize() {
  _scene.loadXMLFile("Scene.xml");
  
  _model = _scene.getModel("Plane");
  
  // Set the Camera Textures
  fx::TrackerSystem &tracker = fx::TrackerSystem::getInstance();
  fx::Material *material = _model->material();
  material->addTexture(tracker.getCameraImageY());
  material->addTexture(tracker.getCameraImageCbCr());
}

void ARTracker::update() {
  if (_model->hidden()) {
    fx::TextureBuffer *firstTexture = _model->material()->textures().begin()->buffer;
    _model->setHidden(!firstTexture->loaded());
  }
  
  _scene.update();
}

void ARTracker::render() {
  _scene.render();
}
