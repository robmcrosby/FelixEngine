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
}

void ARTracker::update() {
  _scene.update();
}

void ARTracker::render() {
  _scene.render();
}
