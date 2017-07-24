//
//  Model.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Model.h"
#include "Scene.h"
#include "Material.h"


using namespace fx;
using namespace std;


Model::Model(Scene *scene): _scene(scene), _data(1), _material(nullptr), _mesh(nullptr), _hidden(false) {
}

Model::~Model() {

}

void Model::update() {
  _data[0].model = mat4::Trans3d(_position) * _orientation.toMat4() * mat4::Scale(vec3(_scale, _scale, _scale));
  _data[0].rotation = _orientation;
}

bool Model::loadXML(const XMLTree::Node &node) {
  bool success = true;
  
  if (node.hasAttribute("mesh"))
    setMesh(node.attribute("mesh"));
  if (node.hasAttribute("material"))
    setMaterial(node.attribute("material"));
  if (node.hasAttribute("pass"))
    _scene->renderPasses()[node.attribute("pass")].addModel(this);
  
  for (auto subNode : node) {
    if (*subNode == "Transform")
      success &= setTransform(*subNode);
    //TODO: add sub models, Meshes, Shaders, etc.
  }
  return success;
}

void Model::setMesh(const string &name) {
  setMesh(_scene->getMesh(name));
}

void Model::setMaterial(const string &name) {
  setMaterial(_scene->getMaterial(name));
}

void Model::setShader(ShaderProgram *shader) {
  if (_material == nullptr)
    _material = _scene->getMaterial();
  _material->setShader(shader);
}

bool Model::setTransform(const XMLTree::Node &node) {
  if (node.hasAttribute("position"))
    setPosition(node.attribute("position"));
  else if (node.hasAttribute("orientation"))
    setOrientation(node.attribute("orientation"));
  else if (node.hasAttribute("scale"))
    setScale(node.attributeAsFloat("scale"));
  //update();
  return true;
}

void Model::enableDepthTesting() {
  if (_material == nullptr)
    _material = _scene->getMaterial();
  _material->enableDepthTesting();
}
