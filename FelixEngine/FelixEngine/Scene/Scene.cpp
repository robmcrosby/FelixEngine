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
    model->update();
  for (auto camera : _cameras)
    camera->update();
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
    if (*subNode == "FrameBuffer")
      success &= addFrame(*subNode);
    else if (*subNode == "ShaderProgram")
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

bool Scene::addFrame(const XMLTree::Node &node) {
  string name = node.attribute("name");
  if (node.hasAttribute("window")) {
    int index = node.attributeAsInt("window");
    return getWindow(name, index)->loadXML(node);
  }
  return getFrame(name)->loadXML(node);
}

bool Scene::addShader(const XMLTree::Node &node) {
  return getShader(node.attribute("name"))->loadXML(node);
}

bool Scene::addMesh(const XMLTree::Node &node) {
  return getMesh(node.attribute("name"))->loadXML(node);
}

bool Scene::addLightRig(const XMLTree::Node &node) {
  return getLightRig(node.attribute("name"))->loadXML(node);
}

bool Scene::addMaterial(const XMLTree::Node &node) {
  return getMaterial(node.attribute("name"))->loadXML(node);
}

bool Scene::addCamera(const XMLTree::Node &node) {
  if (node != "Camera")
    return false;
  return getCamera(node.attribute("name"))->loadXML(node);
}

bool Scene::addModel(const XMLTree::Node &node) {
  if (node != "Model")
    return false;
  return getModel(node.attribute("name"))->loadXML(node);
}

FrameBuffer* Scene::getWindow(const std::string &name, int index) {
  if (_frames.contains(name))
    return _frames[name];
  FrameBuffer *buffer = Graphics::getInstance().getMainWindowBuffer();
  _frames.push(name, buffer);
  return buffer;
}

FrameBuffer* Scene::getFrame(const std::string &name) {
  if (_frames.contains(name))
    return _frames[name];
  FrameBuffer *buffer = Graphics::getInstance().createFrameBuffer();
  _frames.push(name, buffer);
  return buffer;
}

ShaderProgram* Scene::getShader(const std::string &name) {
  if (_shaders.contains(name))
    return _shaders[name];
  else {
    ShaderProgram *shader = Graphics::getInstance().createShaderProgram();
    _shaders.push(name, shader);
    return shader;
  }
}

VertexMesh* Scene::getMesh(const std::string &name) {
  if (_meshes.contains(name))
    return _meshes[name];
  else {
    VertexMesh *mesh = Graphics::getInstance().createVertexMesh();
    _meshes.push(name, mesh);
    return mesh;
  }
}

Model* Scene::getModel(const std::string &name) {
  if (_models.contains(name))
    return _models[name];
  else {
    Model *model = new Model(this);
    _models.push(name, model);
    return model;
  }
}

Camera* Scene::getCamera(const std::string &name) {
  if (_cameras.contains(name))
    return _cameras[name];
  else {
    Camera *camera = new Camera(this);
    _cameras.push(name, camera);
    return camera;
  }
}

Material* Scene::getMaterial(const std::string &name) {
  if (_materials.contains(name))
    return _materials[name];
  else {
    Material *material = new Material(this);
    _materials.push(name, material);
    return material;
  }
}

LightRig* Scene::getLightRig(const std::string &name) {
  if (_lights.contains(name))
    return _lights[name];
  else {
    LightRig *light = new LightRig();
    _lights.push(name, light);
    return light;
  }
}
