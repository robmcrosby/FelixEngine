//
//  Mesh.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Resources.h"
#include "GraphicResources.h"

using namespace fx;
using namespace std;


MeshBuilder Mesh::meshBuilder = MeshBuilder();

bool Mesh::loadXML(const XMLTree::Node &node) {
  _mesh = Graphics::getInstance().getVertexMesh(node.attribute("name"));
  return _mesh->loadXML(node);
}


ShaderBuilder Shader::shaderBuilder = ShaderBuilder();

bool Shader::loadXML(const XMLTree::Node &node) {
  _shader = Graphics::getInstance().getShaderProgram(node.attribute("name"));
  return _shader->loadXML(node);
}


FrameBuilder Frame::frameBuilder = FrameBuilder();

bool Frame::loadXML(const XMLTree::Node &node) {
  if (node.hasAttribute("window"))
    _frame = Graphics::getInstance().getMainWindowBuffer();
  else
    _frame = Graphics::getInstance().getFrameBuffer(node.attribute("name"));
  return _frame->loadXML(node);
}


TextureBuilder Texture::textureBuilder = TextureBuilder();

bool Texture::loadXML(const XMLTree::Node &node) {
  _texture = Graphics::getInstance().getTextureBuffer(node.attribute("name"));
  return _texture->loadXML(node);
}
