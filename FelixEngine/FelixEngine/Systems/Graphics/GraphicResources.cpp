//
//  GraphicResources.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/17/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include "FileSystem.h"
#include "MeshBuilder.h"


using namespace fx;
using namespace std;


bool ShaderProgram::loadXML(const XMLTree::Node &node) {
  if (node.hasAttributes("vertex", "fragment"))
    return loadShaderFunctions(node.attribute("vertex"), node.attribute("fragment"));
  return false;
}


bool VertexMesh::loadXML(const XMLTree::Node &node) {
  VertexMeshData meshData;
  
  if (node.hasAttribute("file")) {
    if (FileSystem::loadMesh(meshData, node.attribute("file")))
      return load(meshData);
  }
  else if (node.hasAttribute("primitive")) {
    return MeshBuilder::loadPrimitiveToMesh(this, node);
  }
  else {
    if (meshData.loadXML(node))
      return load(meshData);
  }
  return false;
}

bool VertexMesh::setVertexBuffer(const string &name, const vector<float> &buffer) {
  return setVertexBuffer(name, 1, buffer.size(), &buffer.at(0));
}

bool VertexMesh::setVertexBuffer(const string &name, const vector<vec2> &buffer) {
  return setVertexBuffer(name, 2, buffer.size(), &buffer.at(0).x);
}

bool VertexMesh::setVertexBuffer(const string &name, const vector<vec4> &buffer) {
  return setVertexBuffer(name, 4, buffer.size(), &buffer.at(0).x);
}


bool TextureBuffer::loadXML(const XMLTree::Node &node) {
  setDefaultSampler(node);
  if (node.hasAttribute("file"))
    return loadFile(node.attribute("file"));
  return false;
}

bool TextureBuffer::loadFile(const std::string &file) {
  ImageBufferData image;
  if (FileSystem::loadImage(image, file))
    return load(image);
  return false;
}

void FrameBuffer::setScene(Scene *scene) {
  _scene = scene;
}

bool FrameBuffer::loadXML(const XMLTree::Node &node) {
  bool success = true;
  if (node.hasAttribute("window")) {
    // Set to Existing Window Texture
    success &= setToWindow(node.attributeAsInt("window"));
  }
  else {
    // Create A Frame Buffer Object
    int width = node.hasAttribute("width") ? node.attributeAsInt("width") : 512;
    int height = node.hasAttribute("height") ? node.attributeAsInt("height") : 512;
    resize(width, height);
  }
  
  for (auto subnode : node) {
    if (subnode->attribute("type") == "depth")
      addDepthBuffer();
    else
      addColorTexture(subnode->attribute("name"));
  }
  return success;
}
