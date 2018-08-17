//
//  Model.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Model.h"
#include "MeshBuilder.h"


using namespace fx;
using namespace std;

ModelBuilder Model::modelBuilder = ModelBuilder();

Model::Model(): _hidden(0) {
  _scene = nullptr;
  _transforms.push_back(Transform::make());
  _modelTransforms.push_back(mat4());
  _textureTransforms.push_back(mat4());
  
  _uniforms = UniformMap::make();
  (*_uniforms)["model"] = STR_Model();
  _renderItem.uniforms.push_back(_uniforms);
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
    setToRenderPass(node.attribute("pass"));
  if (node.hasAttribute("hidden"))
    setHidden(node.attributeAsBoolean("hidden"));
  if (node.hasAttribute("layer"))
    setLayer(node.attributeAsInt("layer"));
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
  cerr << "Unkown XML Node in Model:" << endl << node << endl;
  return false;
}

void Model::update(float dt) {
  Uniform &models = (*_uniforms)["model"];
  models.resize(_transforms.size());
  for (int i = 0; i < _transforms.size(); ++i) {
    STR_Model &model = (STR_Model&)models[i];
    model.model = globalModelTransform(i);
    model.texture = textureTransform(i);
    model.rotation = _transforms.at(i)->globalRotation().toVec4();
  }
  _uniforms->update();
  
  if (renderable()) {
    // Make copy of the Model's Render Item
    RenderItem item = _renderItem;
    
    if (_material) {
      // Set Render Item's properties from the material
      item.shader = _material->shader();
      item.uniforms.push_back(_material->uniforms());
      item.textures = _material->textures();
      item.depthState = _material->depthState();
      item.blendState = _material->blendState();
    }
    else {
      // TODO: Set Render Items's properties with out the material
    }
    
    // Add the Render Item to each of the Render Passes
    for (auto &pass : _renderPasses)
      pass->addRenderItem(item);
  }
}

bool Model::renderable() const {
  if (!_hidden && _renderItem.instances > 0 && _renderItem.mesh && _renderItem.texturesLoaded())
    return _renderItem.shader || (_material && _material->shader());
  return false;
}

void Model::setInstances(unsigned int instances) {
  while (instances > _transforms.size()) {
    _transforms.push_back(Transform::make());
    _modelTransforms.push_back(mat4());
    _textureTransforms.push_back(mat4());
  }
  _renderItem.instances = instances;
  
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
  _renderItem.mesh = Graphics::getInstance().getVertexMesh(node.attribute("name"));
  return _renderItem.mesh->loadXML(node);
}

void Model::setMesh(const std::string &name) {
  _renderItem.mesh = Graphics::getInstance().getVertexMesh(name);
}

void Model::loadMesh(const std::string &file) {
  _renderItem.mesh = MeshBuilder::createFromFile(file);
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

void Model::setModelTransform(const mat4 &transform, int index) {
  if (index < _modelTransforms.size())
    _modelTransforms[index] = transform;
}

mat4 Model::modelTransform(int index) {
  if (index < _modelTransforms.size())
    return _modelTransforms.at(index);
  return mat4();
}

void Model::setTextureTransform(const mat4 &transform, int index) {
  if (index < _textureTransforms.size())
    _textureTransforms[index] = transform;
}

mat4 Model::textureTransform(int index) {
  if (index < _textureTransforms.size())
    return _textureTransforms.at(index);
  return mat4();
}

mat4 Model::globalModelTransform(int index) {
  if (index < _transforms.size()) {
    if (index < _modelTransforms.size())
      return _transforms.at(index)->globalTransform() * _modelTransforms.at(index);
    return _transforms.at(index)->globalTransform();
  }
  return mat4();
}

mat4 Model::localModelTransform(int index) {
  if (index < _transforms.size()) {
    if (index < _modelTransforms.size())
      return _transforms.at(index)->localTransform() * _modelTransforms.at(index);
    return _transforms.at(index)->localTransform();
  }
  return mat4();
}
