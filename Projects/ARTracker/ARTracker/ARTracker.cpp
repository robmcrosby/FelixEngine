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
  
}

ARTracker::~ARTracker() {
  
}

void ARTracker::initalize() {
  _scene.loadXMLFile("Scene.xml");
  _renderScheme.push_back("MainPass");
}

void ARTracker::update(float td) {
  _scene.update(td);
  
  fx::RenderPass::renderPasses(_renderScheme);
  fx::RenderPass::resetPasses();
}
