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

Light::Light() {
  _transform = Transform::make();
  _energy = 0.0f;
  _distance = 0.0f;
  
  _linearAttenuation = 0.0f;
  _squareAttenuation = 0.0f;
  
  _coneAngle = 0.0f;
  _softAngle = 0.0f;
  
  _directional = false;
  
  _layer = 0;
}

Light::~Light() {
  
}

bool Light::loadXML(const XMLTree::Node &node) {
  bool success = true;
  
  if (node.hasAttribute("pass"))
    addToRenderPass(node.attribute("pass"));
  if (node.hasAttribute("energy"))
    setEnergy(node.attributeAsFloat("energy"));
  if (node.hasAttribute("type")) {
    if (node.attribute("type") == "directional")
      setAsDirectionalLight();
    else
      setAsPointLight();
  }
  
  for (auto &subNode : node)
    success &= loadXMLItem(*subNode);
  return success;
}

bool Light::loadXMLItem(const XMLTree::Node &node) {
  if (node == "Transform")
    return _transform->loadXML(node);
  if (node == "Color") {
    setColor(node.contents());
    return true;
  }
  cerr << "Unknown XML: " << node.element() << " in Light" << endl;
  return false;
}

void Light::update(float dt) {
  for (auto pass = _renderPasses.begin(); pass != _renderPasses.end(); ++pass)
    (*pass)->addLight(this);
}

void Light::applyToTask(GraphicTask &task) {
  
}
