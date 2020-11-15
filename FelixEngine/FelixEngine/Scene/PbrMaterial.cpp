//
//  PbrMaterial.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/15/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "PbrMaterial.h"


using namespace fx;
using namespace std;


PbrMaterialBuilder PbrMaterial::materialBuilder = PbrMaterialBuilder();


PbrMaterial::PbrMaterial(): Material() {
}

PbrMaterial::~PbrMaterial() {
  
}

bool PbrMaterial::load(const XMLTree::Node &node) {
  bool success = true;
  if (node.hasAttribute("shader"))
    setShader(node.attribute("shader"));
  
  //for (auto subNode : node)
  //  success &= loadXMLItem(*subNode);
  return success;
}

bool PbrMaterial::load(const USDCrate &crate, const string &path) {
  //cout << "Load Crate: " << path;
//  crate.printUSD();
//  string shaderPath = crate.getShaderPath(path);
//  string diffusePath = crate.getInputPath(shaderPath, "diffuseColor");
//  cout << "diffuseColor: " << diffusePath << endl;
//  cout << "is Texture: " << crate.isTextureInput(shaderPath, "diffuseColor") << endl;
//  cout << "diffuseTexture: " << crate.getTexturePath(shaderPath, "diffuseColor") <<  endl;

  _shader = Graphics::getInstance().getShaderProgram("TestShader");
  _shader->loadShaderFunctions("v_texture_normal", "f_pbr_shadeless");

  _depthState.enableDefaultTesting();

  return true;
}

void PbrMaterial::update(float dt) {
  Material::update(dt);
}
