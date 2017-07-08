//
//  Scene.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Scene.h"


using namespace fx;
using namespace std;

Scene::Scene() {
  
}

Scene::~Scene() {
  
}

void Scene::update() {
  
}

void Scene::render() {
  
}


Model* getModel(const std::string &name) {
  return nullptr;
}

Model* setModel(Model *model, const std::string &name) {
  return model;
}

Camera* getCamera(const std::string &name) {
  return nullptr;
}

Camera* setCamera(Camera *camera, const std::string &name) {
  return camera;
}

Material* getMaterial(const std::string &name) {
  return nullptr;
}

Material* setMaterial(Material *material, const std::string &name) {
  return material;
}

LightRig* getLightRig(const std::string &name) {
  return nullptr;
}

LightRig* setLightRig(LightRig *light, const std::string &name) {
  return light;
}
