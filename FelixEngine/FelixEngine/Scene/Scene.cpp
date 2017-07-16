//
//  Scene.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Scene.h"
#include "Model.h"
#include "Camera.h"
#include "Material.h"
#include "LightRig.h"


using namespace fx;
using namespace std;

Scene::Scene() {
  
}

Scene::~Scene() {
  
}

void Scene::update() {
  for (auto model : _models)
    model.second->update();
  for (auto camera : _cameras)
    camera.second->update();
  _renderPasses.update();
}

void Scene::render() {
  _renderPasses.render();
}


Model* Scene::getModel(const std::string &name) {
  if (_models.count(name) == 0)
    _models[name] = new Model();
  return _models[name];
}

Model* Scene::setModel(Model *model, const std::string &name) {
  if (_models.count(name) == 0 && _models.at(name) != model)
    delete _models.at(name);
  return _models[name] = model;
}

Camera* Scene::getCamera(const std::string &name) {
  if (_cameras.count(name) == 0)
    _cameras[name] = new Camera();
  return _cameras[name];
}

Camera* Scene::setCamera(Camera *camera, const std::string &name) {
  if (_cameras.count(name) == 0 && _cameras.at(name) != camera)
    delete _cameras.at(name);
  return _cameras[name] = camera;
}

Material* Scene::getMaterial(const std::string &name) {
  if (_materials.count(name) == 0)
    _materials[name] = new Material();
  return _materials[name];
}

Material* Scene::setMaterial(Material *material, const std::string &name) {
  if (_materials.count(name) == 0 && _materials.at(name) != material)
    delete _materials.at(name);
  return _materials[name] = material;
}

LightRig* Scene::getLightRig(const std::string &name) {
  if (_lights.count(name) == 0)
    _lights[name] = new LightRig();
  return _lights[name];
}

LightRig* Scene::setLightRig(LightRig *light, const std::string &name) {
  if (_lights.count(name) == 0 && _lights.at(name) != light)
    delete _lights.at(name);
  return _lights[name] = light;
}
