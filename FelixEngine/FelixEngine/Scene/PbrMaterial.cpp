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
TextureBufferPtr PbrMaterial::spbrdfLut = TextureBufferPtr();


PbrMaterial::PbrMaterial(): Material() {
  _textures = TextureMap::make();
  if (!spbrdfLut) {
    spbrdfLut = Graphics::getInstance().createTextureBuffer();
    spbrdfLut->loadSpbrdfLut();
  }
  _textures->setTexture("spbrdfLut", spbrdfLut);
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

bool PbrMaterial::load(const USDItem &item) {
  //cout << "Load Material Item" << endl;
  //cout << item << endl;
  
  const USDItem *shaderItem = item.getConnectedItem("outputs:surface");
  if (shaderItem) {
    //cout << "Shader Item" << endl;
    //cout << *shaderItem << endl;
    
    setShader(shaderItem->stringValue("info:id"));
    
    addTexture("albedoTexture", shaderItem->getAttribute("inputs:diffuseColor"));
    addTexture("specularTexture", shaderItem->getAttribute("inputs:specularColor"));
    addTexture("roughnessTexture", shaderItem->getAttribute("inputs:roughness"));
    addTexture("metalnessTexture", shaderItem->getAttribute("inputs:metallic"));
    addTexture("emissiveTexture", shaderItem->getAttribute("inputs:emissiveColor"));
    addTexture("occlusionTexture", shaderItem->getAttribute("inputs:occlusion"), RGBA(255, 255, 255, 255));
    addTexture("normalsTexture", shaderItem->getAttribute("inputs:normal"), RGBA(127, 127, 255, 255));
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
  addTexture(name, input, &def);
}

void PbrMaterial::addTexture(const string &name, const USDItem *input, RGBA *def) {
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
        _textures->setColor(name, textureItem->getColorValue("inputs:fallback", def ? *def : RGBA(0, 0, 0, 255)));
    }
    else
      _textures->setColor(name, def ? *def : input->getColorValue(RGBA(0, 0, 0, 255)));
  }
  else
    _textures->setColor(name, def ? *def : RGBA(0, 0, 0, 255));
}
