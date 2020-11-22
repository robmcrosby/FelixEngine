//
//  PbrMaterial.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/15/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#include "PbrMaterial.h"
#include "USDCrate.h"


using namespace fx;
using namespace std;


PbrMaterialBuilder PbrMaterial::materialBuilder = PbrMaterialBuilder();


PbrMaterial::PbrMaterial(): Material() {
  if (!_textures)
    _textures = TextureMap::make();
}

PbrMaterial::~PbrMaterial() {
  
}

bool PbrMaterial::load(const XMLTree::Node &node) {
  bool success = Material::load(node);
  //if (node.hasAttribute("shader"))
  //  setShader(node.attribute("shader"));
  
  //for (auto subNode : node)
  //  success &= loadXMLItem(*subNode);
  return success;
}

bool PbrMaterial::load(const USDCrate &crate, const string &path) {
  //cout << "Load Crate: " << path;
  crate.printUSD();

  _shader = Graphics::getInstance().getShaderProgram("TestShader");
  _shader->loadShaderFunctions("v_texture_normal", "f_pbr_shadeless");
  
  string shaderPath = crate.getShaderPath(path);
  
  if (crate.isTextureInput(shaderPath, "diffuseColor")) {
    string textureFile = crate.getTextureFile(shaderPath, "diffuseColor");
    TextureBufferPtr texture = _scene->getTextureBuffer(textureFile);
    _textures->setTexture("diffuseColor", texture);
  }
  else {
    RGBA color = crate.getColorInput(shaderPath, "diffuseColor");
    _textures->setColor("diffuseColor", color);
  }
  
  //_textures->setColor("diffuseColor", RGBA(0, 0, 255, 255));

  _depthState.enableDefaultTesting();

  return true;
}

void PbrMaterial::update(float dt) {
  Material::update(dt);
}
