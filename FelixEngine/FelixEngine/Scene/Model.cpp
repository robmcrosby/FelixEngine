//
//  Model.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Model.h"
#include "RenderPass.h"


using namespace fx;
using namespace std;


ModelBuilder Model::modelBuilder = ModelBuilder();

Model::Model(): _layer(0) {
  _scene = nullptr;
  _instances = 1;
  _transforms.push_back(Transform::make());
  _uniforms = UniformMap::make();
}

Model::~Model() {
  
}

bool Model::loadXML(const XMLTree::Node &node) {
  bool success = true;
  if (node.hasAttribute("mesh"))
    setMesh(node.attribute("mesh"));
  if (node.hasAttribute("material"))
    setMaterial(node.attribute("material"));
  if (node.hasAttribute("pass"))
    addToRenderPass(node.attribute("pass"));
  if (node.hasAttribute("hidden"))
    setHidden(node.attributeAsBoolean("hidden"));
  
  for (auto subNode : node)
    success &= loadXMLItem(*subNode);
  return success;
}

bool Model::loadXMLItem(const XMLTree::Node &node) {
  if (node == "Transform" && _transforms.size() > 0)
    return _transforms.front()->loadXML(node);
  if (node == "Mesh")
    return setMesh(node);
  if (node == "Material")
    return setMaterial(node);
  return false;
}

void Model::update(float dt) {
  vector<STR_Model> transforms(_transforms.size());
  for (int i = 0; i < _transforms.size(); ++i) {
    transforms[i].model = _transforms[i]->globalTransform();
    transforms[i].rotation = _transforms[i]->globalRotation().toVec4();
  }
  (*_uniforms)["model"] = transforms;
  _uniforms->update();
  
  if (!_hidden && _instances > 0) {
    for (auto pass = _renderPasses.begin(); pass != _renderPasses.end(); ++pass)
      (*pass)->addModel(this);
  }
}

void Model::applyToTask(fx::GraphicTask &task) {
  task.uniforms.push_back(_uniforms);
  task.mesh = _mesh;
  task.instances = _instances;
  
  if (_material)
    _material->setToTask(task);
}

void Model::setInstances(unsigned int instances) {
  while (instances > _transforms.size())
    _transforms.push_back(Transform::make());
  _instances = instances;
  
  if (!(*_uniforms)["model"].usingBuffer() && instances > 4) {
    (*_uniforms)["model"].useBuffer();
  }
}

bool Model::setMaterial(const XMLTree::Node &node) {
  _material = _scene->get<Material>(node.attribute("name"));
  return _material->loadXML(node);
}

void Model::setMaterial(const std::string &name) {
  if (_scene)
    _material = _scene->get<Material>(name);
}

bool Model::setMesh(const XMLTree::Node &node) {
  _mesh = Graphics::getInstance().getVertexMesh(node.attribute("name"));
  return _mesh->loadXML(node);
}

void Model::setMesh(const std::string &name) {
  _mesh = Graphics::getInstance().getVertexMesh(name);
}

void Model::setScale(float scale, int index) {
  if (index < _transforms.size())
    _transforms[index]->setScale(scale);
}

void Model::setScale(const vec3 &scale, int index) {
  if (index < _transforms.size())
    _transforms[index]->setScale(scale);
}

vec3 Model::localScale(int index) const {
  if (index < _transforms.size())
    return _transforms[index]->localScale();
  return vec3(1.0f, 1.0f, 1.0f);
}

void Model::setRotation(const quat &orientation, int index) {
  if (index < _transforms.size())
    _transforms[index]->setRotation(orientation);
}

quat Model::localRotation(int index) const {
  if (index < _transforms.size())
    return _transforms[index]->localRotation();
  return quat();
}

void Model::setLocation(const vec3 &location, int index) {
  if (index < _transforms.size())
    _transforms[index]->setLocation(location);
  
}
vec3 Model::localLocation(int index) const {
  if (index < _transforms.size())
    return _transforms[index]->localLocation();
  return vec3();
}

TransformPtr Model::transform(int index) {
  if (index < _transforms.size())
    return _transforms[index];
  return Transform::make();
}
