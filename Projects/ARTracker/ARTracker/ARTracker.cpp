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


ARTracker::ARTracker() {
  printf("Create ARTracker\n");
}

ARTracker::~ARTracker() {
  
}

void ARTracker::initalize() {
  _scene.loadXMLFile("Scene.xml");
  _model = _scene.getModel("Model");
}

void ARTracker::update() {
  //_model->setOrientation(_model->orientation() * fx::quat::RotZ(0.02f));
  //_model->update();
  
  _scene.update();
}

void ARTracker::render() {
  _scene.render();
}
