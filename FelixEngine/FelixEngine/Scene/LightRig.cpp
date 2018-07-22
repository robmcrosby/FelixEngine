//
//  LightRig.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/21/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "LightRig.h"


using namespace fx;
using namespace std;

LightRigBuilder LightRig::lightRigBuilder = LightRigBuilder();


LightRig::LightRig() {
  
}

LightRig::~LightRig() {
  
}

bool LightRig::loadXML(const XMLTree::Node &node) {
  // TODO
  return false;
}

void LightRig::update(float dt) {
  // Create the Light Parameters
  vector<STR_Light> parameters;
  for (auto &light : _lights)
    parameters.push_back(light->peramaters());
  
  // Set the Light Parameters to Each Render Pass
  for (auto &pass : _renderPasses)
    pass->getUniformMap()["lights"] = parameters;
}

void LightRig::addDirectionalLight(vec3 direction, vec3 color, float energy) {
  LightPtr light = _scene->get<Light>("");
  light->setAsDirectionalLight(direction, color, energy);
  addLight(light);
}

void LightRig::addPointLight(vec3 location, vec3 color, float energy) {
  LightPtr light = _scene->get<Light>("");
  light->setAsPointLight(location, color, energy);
  addLight(light);
}

void LightRig::addLight(LightPtr light) {
  _lights.push_back(light);
}

LightPtr LightRig::getLight(int index) {
  return _lights.at(index);
}

void LightRig::clearLights() {
  _lights.clear();
}
