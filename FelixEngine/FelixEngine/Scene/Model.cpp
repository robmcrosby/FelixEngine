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
  _scale = vec3(1.0f, 1.0f, 1.0f);
  _uniforms = make_shared<UniformMap>();
  (*_uniforms)["model"] = _transform;
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
  if (node == "Transform")
    return setTransform(node);
  if (node == "Mesh")
    return setMesh(node);
  if (node == "Material")
    return setMaterial(node);
  return false;
}

bool Model::setTransform(const XMLTree::Node &node) {
  if (node.hasAttribute("position"))
    setPosition(node.attribute("position"));
  if (node.hasAttribute("orientation"))
    setOrientation(node.attribute("orientation"));
  if (node.hasAttribute("scale"))
    setScale(node.attributeAsFloat("scale"));
  return true;
}

void Model::update(float dt) {
  _transform.model = mat4::Trans3d(_position) * _orientation.toMat4() * mat4::Scale(_scale);
  _transform.rotation = _orientation;
  (*_uniforms)["model"] = _transform;
  
  if (!_hidden) {
    for (auto pass = _renderPasses.begin(); pass != _renderPasses.end(); ++pass)
      (*pass)->addModel(this);
  }
}

void Model::applyToTask(fx::GraphicTask &task) {
  task.uniforms.push_back(_uniforms);
  task.mesh = _mesh;
  
  if (_material)
    _material->setToTask(task);
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
