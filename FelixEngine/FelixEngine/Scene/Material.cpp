//
//  Material.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/18/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Material.h"
#include "GraphicTask.h"


using namespace fx;
using namespace std;

MaterialBuilder Material::materialBuilder = MaterialBuilder();

Material::Material(): _scene(0) {
  _uniforms = make_shared<UniformMap>();
  (*_uniforms)["material"] = _properties;
}

Material::~Material() {
  
}

bool Material::loadXML(const XMLTree::Node &node) {
  bool success = true;
  
  if (node.hasAttribute("shader"))
    setShader(node.attribute("shader"));
  
  for (auto subNode : node) {
    if (subNode->element() == "Ambiant")
      success &= setAmbiant(*subNode);
    else if (subNode->element() == "Diffuse")
      success &= setDiffuse(*subNode);
    else if (subNode->element() == "Specular")
      success &= setSpecular(*subNode);
    else if (subNode->element() == "DepthState")
      success &= _depthState.loadXml(*subNode);
    else if (subNode->element() == "BlendState")
      success &= _blendState.loadXml(*subNode);
  }
  return success;
}

void Material::update(float dt) {
  (*_uniforms)["material"] = _properties;
}

void Material::setToTask(GraphicTask &task) {
  task.uniforms.push_back(_uniforms);
  task.textures = _textures;
  task.shader = _shader;
  task.depthState = _depthState;
  task.blendState = _blendState;
}

void Material::setShader(const string &name) {
  _shader = Graphics::getInstance().getShaderProgram(name);
}

void Material::addTexture(TexturePtr &texture, SamplerState sampler) {
  _textures->addTexture(texture, sampler);
}

bool Material::setAmbiant(const XMLTree::Node &node) {
  if (node.hasAttributes("color", "factor")) {
    setAmbiant(node.attribute("color"), node.attributeAsFloat("factor"));
    return true;
  }
  return false;
}

bool Material::setDiffuse(const XMLTree::Node &node) {
  if (node.hasAttributes("color", "factor")) {
    setDiffuse(node.attribute("color"), node.attributeAsFloat("factor"));
    return true;
  }
  return false;
}

void Material::setSpecular(const vec3 &color, float factor, float hardness) {
  _properties.specular = vec4(color, factor);
  _properties.factors.x = hardness;
}

bool Material::setSpecular(const XMLTree::Node &node) {
  if (node.hasAttributes("color", "factor", "hardness")) {
    setSpecular(node.attribute("color"), node.attributeAsFloat("factor"), node.attributeAsFloat("hardness"));
    return true;
  }
  return false;
}
