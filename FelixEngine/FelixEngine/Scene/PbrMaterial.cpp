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

//bool PbrMaterial::load(const USDCrate &crate, const string &path) {
//  //cout << "Load Crate: " << path;
//  //cout << crate << endl;
//
//  _shader = Graphics::getInstance().getShaderProgram("TestShader");
//  _shader->loadShaderFunctions("v_texture_normal", "f_pbr_shadeless");
//
//  string shaderPath = crate.getShaderPath(path);
//
//  if (crate.isTextureInput(shaderPath, "diffuseColor")) {
//    string textureFile = crate.getTextureFile(shaderPath, "diffuseColor");
//    TextureBufferPtr texture = _scene->getTextureBuffer(textureFile);
//    _textures->setTexture("diffuseColor", texture);
//  }
//  else {
//    RGBA color = crate.getColorInput(shaderPath, "diffuseColor");
//    _textures->setColor("diffuseColor", color);
//  }
//
//  //_textures->setColor("diffuseColor", RGBA(0, 0, 255, 255));
//
//  _depthState.enableDefaultTesting();
//
//  return true;
//}

bool PbrMaterial::load(const USDItem &item) {
  cout << "Load Material Item" << endl;
  cout << item << endl;
  
  _shader = Graphics::getInstance().getShaderProgram("TestShader");
  _shader->loadShaderFunctions("v_texture_normal", "f_pbr_shadeless");
  
  const USDItem *shaderItem = item.getConnectedItem("outputs:surface");
  if (shaderItem) {
    cout << "Shader Item" << endl;
    cout << *shaderItem << endl;
    
    addTexture("diffuseColor", shaderItem->getAttribute("inputs:diffuseColor"), RGBA(0, 0, 0, 255));
    //addTexture("diffuseColor", shaderItem->getAttribute("inputs:normal"), RGBA(127, 255, 127, 255));
    //addTexture("diffuseColor", shaderItem->getAttribute("inputs:metallic"), RGBA(0, 0, 0, 255));
    //addTexture("diffuseColor", shaderItem->getAttribute("inputs:specularColor"), RGBA(0, 0, 0, 255));
  }
  else {
    _textures->setColor("diffuseColor", RGBA(0, 0, 255, 255));
  }
  
  _depthState.enableDefaultTesting();
  
  return true;
}

void PbrMaterial::update(float dt) {
  Material::update(dt);
}

void PbrMaterial::addTexture(const string &name, const USDItem *input, RGBA def) {
  if (input) {
    const USDItem *textureItem = input->getConnectedItem();
    if (textureItem) {
      string textureName = textureItem->stringValue("inputs:file");
      if (textureName != "") {
        TextureBufferPtr texture = _scene->getTextureBuffer(textureName);
        SamplerState sampler;
        sampler.setSCoord(COORD_REPEAT);
        sampler.setTCoord(COORD_REPEAT);
        sampler.setMinFilter(FILTER_LINEAR);
        sampler.setMagFilter(FILTER_LINEAR);
        _textures->setTexture(name, texture, sampler);
      }
      else
        _textures->setColor(name, textureItem->getColorValue("inputs:fallback", def));
    }
    else
      _textures->setColor(name, input->getColorValue(def));
  }
  else
    _textures->setColor(name, def);
}
