//
//  Material.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/18/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

//#include "Material.h"
//#include "Scene.h"
//
//
//using namespace fx;
//using namespace std;
//
//
//Material::Material(Scene *scene): _scene(scene) {
//  _textures = make_shared<TextureMap>();
//}
//
//Material::~Material() {}
//
//bool Material::loadXML(const XMLTree::Node &node) {
//  bool success = true;
//  
//  if (node.hasAttribute("shader"))
//    setShader(node.attribute("shader"));
//  
//  for (auto subNode : node) {
//    if (subNode->element() == "Ambiant")
//      success &= setAmbiant(*subNode);
//    else if (subNode->element() == "Diffuse")
//      success &= setDiffuse(*subNode);
//    else if (subNode->element() == "Specular")
//      success &= setSpecular(*subNode);
//    else if (subNode->element() == "DepthState")
//      success &= _depthState.loadXml(*subNode);
//    else if (subNode->element() == "BlendState")
//      success &= _blendState.loadXml(*subNode);
//  }
//  return success;
//}
//
//void Material::setShader(const string &name) {
//  ShaderPtr shader = _scene->getShader(name);
//  setShader(shader);
//}
//
//void Material::addTexture(TexturePtr &texture, SamplerState sampler) {
//  _textures->addTexture(texture, sampler);
//}
//
//bool Material::setAmbiant(const XMLTree::Node &node) {
//  if (node.hasAttributes("color", "factor")) {
//    setAmbiant(node.attribute("color"), node.attributeAsFloat("factor"));
//    return true;
//  }
//  return false;
//}
//
//bool Material::setDiffuse(const XMLTree::Node &node) {
//  if (node.hasAttributes("color", "factor")) {
//    setDiffuse(node.attribute("color"), node.attributeAsFloat("factor"));
//    return true;
//  }
//  return false;
//}
//
//void Material::setSpecular(const vec3 &color, float factor, float hardness) {
//  _data.specular = vec4(color, factor);
//  _data.factors.x = hardness;
//}
//
//bool Material::setSpecular(const XMLTree::Node &node) {
//  if (node.hasAttributes("color", "factor", "hardness")) {
//    setSpecular(node.attribute("color"), node.attributeAsFloat("factor"), node.attributeAsFloat("hardness"));
//    return true;
//  }
//  return false;
//}

