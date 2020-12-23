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
  _environmentMap.sampler.enableMipMapping();
  _environmentMap.sampler.setMinFilter(FILTER_LINEAR);
  _environmentMap.sampler.setMagFilter(FILTER_LINEAR);
  _environmentMap.sampler.setMipFilter(FILTER_LINEAR);
}

LightRig::~LightRig() {
  
}

bool LightRig::load(const XMLTree::Node &node) {
  bool success = true;
  if (node.hasAttribute("pass"))
    setToRenderPass(node.attribute("pass"));
  for (auto &subNode : node)
    success &= loadXMLItem(*subNode);
  return success;
}

void LightRig::update(float dt) {
  // Create the Light Parameters
  vector<STR_Light> parameters;
  for (auto &light : _lights)
    parameters.push_back(light->peramaters());
  
  // Set the Light Parameters to Environment Mapp to Each Render Pass
  for (auto &pass : _renderPasses) {
    if (parameters.size() > 0)
      pass->getUniformMap()["lights"] = parameters;
    if (_environmentMap.buffer)
      pass->getTextureMap()["environmentMap"] = _environmentMap;
  }
}

bool LightRig::loadXMLItem(const XMLTree::Node &node) {
  if (node == "Light")
    return addLight(node);
  if (node == "RenderPass")
    return setToRenderPass(node);
  if (node == "EnvironmentMap")
    return setEnvironmentMap(node);
  cerr << "Unknown XML Node in LightRig: " << endl << node << endl;
  return false;
}

bool LightRig::setEnvironmentMap(const XMLTree::Node &node) {
  return _environmentMap.loadCubeMap(node);
}

bool LightRig::addLight(const XMLTree::Node &node) {
  LightPtr light = _scene->get<Light>(node.attribute("name"));
  addLight(light);
  return light->load(node);
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
