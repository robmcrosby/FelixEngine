//
//  Material.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/18/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Material.h"


using namespace fx;
using namespace std;

MaterialBuilder Material::materialBuilder = MaterialBuilder();

Material::Material(): _scene(0) {
  _uniforms = UniformMap::make();
  (*_uniforms)["material"] = _properties;
}

Material::~Material() {
  
}

bool Material::loadXML(const XMLTree::Node &node) {
  bool success = true;
  if (node.hasAttribute("shader"))
    setShader(node.attribute("shader"));
  
  for (auto subNode : node)
    success &= loadXMLItem(*subNode);
  return success;
}

bool Material::loadXMLItem(const XMLTree::Node &node) {
  if (node == "Ambiant")
    return setAmbiant(node);
  if (node == "Diffuse")
    return setDiffuse(node);
  if (node == "Specular")
    return setSpecular(node);
  if (node == "DepthState")
    return _depthState.loadXml(node);
  if (node == "BlendState")
    return _blendState.loadXml(node);
  if (node == "Shader")
    return setShader(node);
  if (node == "Texture")
    return addTexture(node);
  if (node == "Textures")
    return addTexture(node);
  return false;
}

void Material::update(float dt) {
  (*_uniforms)["material"] = _properties;
}

//void Material::setToTask(GraphicTask &task) {
//  task.uniforms.push_back(_uniforms);
//  task.textures = _textures;
//  task.shader = _shader;
//  task.depthState = _depthState;
//  task.blendState = _blendState;
//}

bool Material::setShader(const XMLTree::Node &node) {
  _shader = Graphics::getInstance().getShaderProgram(node.attribute("name"));
  return _shader->loadXML(node);
}

void Material::setShader(const string &name) {
  _shader = Graphics::getInstance().getShaderProgram(name);
}

bool Material::loadShader(const string &vertex, const string &fragment) {
  _shader = Graphics::getInstance().createShaderProgram();
  return _shader->loadShaderFunctions(vertex, fragment);
}

bool Material::addTexture(const XMLTree::Node &node) {
  if (!_textures)
    _textures = TextureMap::make();
  return _textures->addTexture(node);
}

bool Material::addTextures(const XMLTree::Node &node) {
  if (!_textures)
    _textures = TextureMap::make();
  return _textures->loadXML(node);
}

void Material::addTexture(TextureBufferPtr &texture, SamplerState sampler) {
  if (!_textures)
    _textures = TextureMap::make();
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
  if (node.hasAttribute("size"))
    setDiffuseSize(node.attributeAsFloat("size"));
  if (node.hasAttribute("smooth"))
    setDiffuseSmooth(node.attributeAsFloat("smooth"));
  if (node.hasAttribute("roughness"))
    setDiffuseRoughness(node.attributeAsFloat("roughness"));
  if (node.hasAttribute("albedo"))
    setDiffuseAlbedo(node.attributeAsFloat("albedo"));
  if (node.hasAttributes("color", "factor")) {
    setDiffuse(node.attribute("color"), node.attributeAsFloat("factor"));
    return true;
  }
  return false;
}

void Material::setSpecular(const vec3 &color, float factor) {
  _properties.specular = vec4(color, factor);
}

bool Material::setSpecular(const XMLTree::Node &node) {
  if (node.hasAttribute("size"))
    setSpecularSize(node.attributeAsFloat("size"));
  if (node.hasAttribute("smooth"))
    setSpecularSmooth(node.attributeAsFloat("smooth"));
  if (node.hasAttribute("hardness"))
    setSpecularHardness(node.attributeAsFloat("hardness"));
  if (node.hasAttributes("color", "factor")) {
    setSpecular(node.attribute("color"), node.attributeAsFloat("factor"));
    return true;
  }
  return false;
}
