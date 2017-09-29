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
  
  for (auto subNode : node) {
    if (*subNode == "Transform")
      success &= setTransform(*subNode);
    //TODO: add sub models, Meshes, Shaders, etc.
  }
  return success;
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
  for (auto pass = _renderPasses.begin(); pass != _renderPasses.end(); ++pass)
    (*pass)->addModel(this);
}

void Model::applyToTask(fx::GraphicTask &task) {
  task.uniforms.push_back(_uniforms);
  task.mesh = _mesh;
  
  if (_material)
    _material->setToTask(task);
}

void Model::setMaterial(const std::string &name) {
  if (_scene)
    _material = _scene->get<Material>(name);
}

void Model::setMesh(const std::string &name) {
  _mesh = Graphics::getInstance().getVertexMesh(name);
}
