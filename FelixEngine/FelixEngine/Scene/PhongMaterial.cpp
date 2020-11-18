//
//  PhongMaterial.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/11/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "PhongMaterial.h"


using namespace fx;
using namespace std;


PhongMaterialBuilder PhongMaterial::materialBuilder = PhongMaterialBuilder();


PhongMaterial::PhongMaterial(): Material() {
  (*_uniforms)["material"] = _properties;
}

PhongMaterial::~PhongMaterial() {
  
}

bool PhongMaterial::load(const USDCrate &crate, const string &path) {
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

void PhongMaterial::update(float dt) {
  Material::update(dt);
  (*_uniforms)["material"] = _properties;
}

bool PhongMaterial::loadItem(const XMLTree::Node &node) {
  if (node == "Ambiant")
    return setAmbiant(node);
  if (node == "Diffuse")
    return setDiffuse(node);
  if (node == "Specular")
    return setSpecular(node);
  return Material::loadItem(node);
}

bool PhongMaterial::setAmbiant(const XMLTree::Node &node) {
  if (node.hasAttributes("color", "factor")) {
    setAmbiant(node.attribute("color"), node.attributeAsFloat("factor"));
    return true;
  }
  return false;
}

bool PhongMaterial::setDiffuse(const XMLTree::Node &node) {
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

void PhongMaterial::setSpecular(const vec3 &color, float factor) {
  _properties.specular = vec4(color, factor);
}

bool PhongMaterial::setSpecular(const XMLTree::Node &node) {
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
