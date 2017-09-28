//
//  Light.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 9/23/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Light.h"
#include "RenderPass.h"


using namespace fx;
using namespace std;

Light::Light(): _layer(0) {
  
}

Light::~Light() {
  
}

bool Light::loadXML(const XMLTree::Node &node) {
  return true;
}

void Light::update(float dt) {
  for (auto pass = _renderPasses.begin(); pass != _renderPasses.end(); ++pass)
    (*pass)->addLight(this);
}

void Light::applyToTask(GraphicTask &task) {
  
}
