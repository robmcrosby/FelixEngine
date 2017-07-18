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
#include "FileSystem.h"


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

bool Scene::loadXMLFile(const std::string &file) {
  XMLTree tree;
  if (FileSystem::loadXMLTree(tree, file)) {
    for (auto node : tree) {
      if (*node == "Scene")
        return loadXML(*node);
    }
  }
  return false;
}

bool Scene::loadXML(const XMLTree::Node &node) {
  bool success = true;
  
  for (auto subNode : node) {
    if (*subNode == "ShaderProgram")
      success &= addShader(*subNode);
    else if (*subNode == "VertexMesh")
      success &= addMesh(*subNode);
    else if (*subNode == "LightRig")
      success &= addLightRig(*subNode);
    else if (*subNode == "Material")
      success &= addMaterial(*subNode);
    else
      success &= addModel(*subNode) || addCamera(*subNode);
  }
  
  return success;
}

bool Scene::addShader(const XMLTree::Node &node) {
  return getShader(node.attribute("name"))->loadXML(node);
}

bool Scene::addMesh(const XMLTree::Node &node) {
  return getMesh(node.attribute("name"))->loadXML(node);
}

bool Scene::addLightRig(const XMLTree::Node &node) {
  //printf("Add Light Rig: %s\n", node.attribute("name").c_str());
  return true;
}

bool Scene::addMaterial(const XMLTree::Node &node) {
  //printf("Add Material: %s\n", node.attribute("name").c_str());
  return true;
}

bool Scene::addCamera(const XMLTree::Node &node) {
  if (node != "Camera")
    return false;
  
  //printf("Add Camera: %s\n", node.attribute("name").c_str());
  return true;
}

bool Scene::addModel(const XMLTree::Node &node) {
  if (node != "Model")
    return false;
  
  //printf("Add Model: %s\n", node.attribute("name").c_str());
  return true;
}

ShaderProgram* Scene::getShader(const std::string &name) {
  if (name != "" && _shaderMap.count(name) > 0)
    return _shaderMap.at(name);
  else {
    ShaderProgram *shader = Graphics::getInstance().createShaderProgram();
    _shaders.insert(shader);
    if (name != "")
      _shaderMap[name] = shader;
    return shader;
  }
}

VertexMesh* Scene::getMesh(const std::string &name) {
  if (name != "" && _mesheMap.count(name) > 0)
    return _mesheMap.at(name);
  else {
    VertexMesh *mesh = Graphics::getInstance().createVertexMesh();
    _meshes.insert(mesh);
    if (name != "")
      _mesheMap[name] = mesh;
    return mesh;
  }
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
