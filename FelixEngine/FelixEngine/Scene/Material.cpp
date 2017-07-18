//
//  Material.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/18/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Material.h"
#include "Scene.h"


using namespace fx;
using namespace std;


Material::Material(Scene *scene): _scene(scene), _shader(nullptr) {
}

Material::~Material() {
  delete _shader;
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
  }
  return success;
}

void Material::setShader(const string &name) {
  setShader(_scene->getShader(name));
}

void Material::addTexture(TextureBuffer *texture, SamplerState sampler) {
  _textures.addTexture(texture, sampler);
}

bool Material::setAmbiant(const XMLTree::Node &node) {
  if (node.hasAttribute("color") && node.hasAttribute("factor")) {
    setAmbiant(node.attribute("color"), node.attributeAsFloat("factor"));
    return true;
  }
  return false;
}

bool Material::setDiffuse(const XMLTree::Node &node) {
  if (node.hasAttribute("color") && node.hasAttribute("factor")) {
    setDiffuse(node.attribute("color"), node.attributeAsFloat("factor"));
    return true;
  }
  return false;
}

void Material::setSpecular(const vec3 &color, float factor, float hardness) {
  _data.specular = vec4(color, factor);
  _data.factors.x = hardness;
}

bool Material::setSpecular(const XMLTree::Node &node) {
  if (node.hasAttribute("color") && node.hasAttribute("factor") && node.hasAttribute("hardness")) {
    setSpecular(node.attribute("color"), node.attributeAsFloat("factor"), node.attributeAsFloat("hardness"));
    return true;
  }
  return false;
}
