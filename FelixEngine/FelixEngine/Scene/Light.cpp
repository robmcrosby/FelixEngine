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

LightBuilder Light::lightBuilder = LightBuilder();

Light::Light(): _layer(0) {
  
}

Light::~Light() {
  
}

bool Light::loadXML(const XMLTree::Node &node) {
  if (node.hasAttribute("pass"))
    addToRenderPass(node.attribute("pass"));
  
  if (node.hasAttributes("color", "energy")) {
    vec3 color = node.attribute("color");
    float energy = node.attributeAsFloat("energy");
    
    if (node.hasAttribute("direction")) {
      vec3 direction = node.attribute("direction");
      setAsDirectionalLight(direction, color, energy);
      return true;
    }
    if (node.hasAttribute("position")) {
      vec3 position = node.attribute("position");
      setAsPointLight(position, color, energy);
      return true;
    }
  }
  return false;
}

void Light::update(float dt) {
  for (auto pass = _renderPasses.begin(); pass != _renderPasses.end(); ++pass)
    (*pass)->addLight(this);
}

void Light::applyToTask(GraphicTask &task) {
  
}
