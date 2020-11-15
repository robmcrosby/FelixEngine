//
//  StdMaterial.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/11/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "StdMaterial.h"


using namespace fx;
using namespace std;


StdMaterialBuilder StdMaterial::materialBuilder = StdMaterialBuilder();


StdMaterial::StdMaterial(): Material() {
  (*_uniforms)["material"] = _properties;
}

StdMaterial::~StdMaterial() {
  
}

bool StdMaterial::load(const XMLTree::Node &node) {
  bool success = true;
  if (node.hasAttribute("shader"))
    setShader(node.attribute("shader"));
  
  for (auto subNode : node)
    success &= loadXMLItem(*subNode);
  return success;
}

bool StdMaterial::load(const USDCrate &crate, const string &path) {
  //cout << "Load Crate: " << path;
//  crate.printUSD();
//  string shaderPath = crate.getShaderPath(path);
//  string diffusePath = crate.getInputPath(shaderPath, "diffuseColor");
//  cout << "diffuseColor: " << diffusePath << endl;
//  cout << "is Texture: " << crate.isTextureInput(shaderPath, "diffuseColor") << endl;
//  cout << "diffuseTexture: " << crate.getTexturePath(shaderPath, "diffuseColor") <<  endl;
  setDiffuse(vec3(1.0, 0.0, 0.0), 1.0);

  _shader = Graphics::getInstance().getShaderProgram("TestShader");
  _shader->loadShaderFunctions("v_normal", "f_lambert_phong_2");

  _depthState.enableDefaultTesting();

  return true;
}

void StdMaterial::update(float dt) {
  Material::update(dt);
  (*_uniforms)["material"] = _properties;
}

bool StdMaterial::loadXMLItem(const XMLTree::Node &node) {
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
    return setTexture(node);
  if (node == "Textures")
    return setTextures(node);
  cerr << "Unknown XML Node in Material:" << endl << node << endl;
  return false;
}

bool StdMaterial::setAmbiant(const XMLTree::Node &node) {
  if (node.hasAttributes("color", "factor")) {
    setAmbiant(node.attribute("color"), node.attributeAsFloat("factor"));
    return true;
  }
  return false;
}

bool StdMaterial::setDiffuse(const XMLTree::Node &node) {
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

void StdMaterial::setSpecular(const vec3 &color, float factor) {
  _properties.specular = vec4(color, factor);
}

bool StdMaterial::setSpecular(const XMLTree::Node &node) {
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
